#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "header.h"
#include "request.h"
#include "response.h"
#include "server.h"

#define MAX_BUFFER_SIZE 2048
#define MAX_HEADER_NUM 48

int sockfd;

struct Server server_constructor(char *PORT) {
  struct Server server;
  server.PORT = PORT;
  return server;
}

void sigchld_handler(int s) {
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

static void start(const char *PORT) {
  struct addrinfo hints, *res, *p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gairv;
  if ((gairv = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gairv));
    exit(1);
  }

  for (p = res; p != NULL; p = p->ai_next) {
    int yes = 1;
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      continue;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(res);

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind to any available socket");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("server: listen");
    exit(1);
  }

  struct sigaction sa;
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
}

char *read_file(char *filename, size_t *file_size) {
  // read in file
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "read_file: error opening file: %s\n", filename);
    return NULL;
  }

  // Calculate the content length
  fseek(fp, 0, SEEK_END);
  *file_size = ftell(fp);
  rewind(fp);

  // allocate memory for file contents
  char *buffer = malloc(*file_size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "read_file: memory allocation failed.\n");
    fclose(fp);
    return NULL;
  }

  // Read the file contents into buffer
  size_t bytes_read = fread(buffer, 1, *file_size, fp);
  if (bytes_read != *file_size) {
    fprintf(stderr, "read_file: error reading file.\n");
    free(buffer);
    fclose(fp);
    return NULL;
  }

  fclose(fp);
  buffer[*file_size] = '\0';

  return buffer;
}

void respond(int *client_fd) {

  char buf[MAX_BUFFER_SIZE];
  recv(*client_fd, buf, MAX_BUFFER_SIZE, 0);

  struct Request request = request_constructor(buf);

  printf("%s %s %s\n", request.method, request.target, request.version);

  char *filename;
  if (strcmp(request.target, "/") == 0) {
    filename = "index.html";
  } else if (strcmp(request.target, "/clicked") == 0) {
    filename = "clicked.html";
  } else {
    filename = "404.html";
  }

  size_t body_length;
  char *body = read_file(filename, &body_length);

  // Create the Content-Length header
  struct Header headers[MAX_HEADER_NUM];
  char contentLength[MAX_BUFFER_SIZE];
  snprintf(contentLength, sizeof(contentLength), "%zu", body_length);
  headers[0] = header_constructor("Content-Length", contentLength);

  // construct response
  struct Response response =
      response_constructor("HTTP/1.1", OK, body, 1, headers);

  // send response string
  char response_string[MAX_BUFFER_SIZE];
  response_to_string(response_string, &response);

  if (send(*client_fd, response_string, strlen(response_string), 0) == -1) {
    perror("send content");
    exit(1);
  }

  free(body);
}

void launch(struct Server *server) {
  struct sockaddr_storage client_addr;
  socklen_t client_addr_len = sizeof client_addr;

  start(server->PORT);

  int client_fd;
  while (1) {
    if ((client_fd = accept(sockfd, (struct sockaddr *)&client_addr,
                            &client_addr_len)) == -1) {
      perror("accept");
      continue;
    }

    if (!fork()) {
      close(sockfd);
      respond(&client_fd);
      close(client_fd);
      exit(0);
    }
    close(client_fd); // parent doesnt need this
  }
}
