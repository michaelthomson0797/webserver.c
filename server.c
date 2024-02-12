#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "request.h"
#include "response.h"
#include "route.h"
#include "server.h"

#define MAX_BUFFER_SIZE 2048
#define MAX_HEADER_NUM 48

int sockfd;

struct Server server_constructor(const char *PORT) {
  struct Server server;
  server.PORT = PORT;
  server.num_routes = 0;
  server.routes = NULL;
  return server;
}

void add_route(struct Server *server, char *method, char *target,
               Route_Callback route_callback) {
  struct Route route = route_constructor(target, method, route_callback);
  server->routes =
      realloc(server->routes, sizeof(struct Route) * (server->num_routes + 1));
  server->routes[server->num_routes++] = route;
}

struct Route *match_route(struct Server *server, char *method, char *target) {
  for (int i = 0; i < server->num_routes; i++) {
    if (strcmp(server->routes[i].target, target) == 0 &&
        strcmp(server->routes[i].method, method) == 0) {
      return &server->routes[i];
    }
  }
  return NULL;
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

char *read_file(char *filename) {
  // read in file
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "read_file: error opening file: %s\n", filename);
    return NULL;
  }

  // Calculate the content length
  fseek(fp, 0, SEEK_END);
  size_t file_size = ftell(fp);
  rewind(fp);

  // allocate memory for file contents
  char *buffer = malloc(file_size + 1);
  if (buffer == NULL) {
    fprintf(stderr, "read_file: memory allocation failed.\n");
    fclose(fp);
    return NULL;
  }

  // Read the file contents into buffer
  size_t bytes_read = fread(buffer, 1, file_size, fp);
  if (bytes_read != file_size) {
    fprintf(stderr, "read_file: error reading file.\n");
    free(buffer);
    fclose(fp);
    return NULL;
  }

  fclose(fp);
  buffer[file_size] = '\0';

  char *value = strdup(buffer);
  free(buffer);

  return value;
}

void respond(struct Server *server, int *client_fd) {
  // Recieve request
  char buf[MAX_BUFFER_SIZE];
  recv(*client_fd, buf, MAX_BUFFER_SIZE, 0);

  // Parse request
  struct Request request = request_constructor(buf);

  // Match route and get body content
  struct Route *matched_route =
      match_route(server, request.method, request.target);

  struct Response response;
  if (matched_route == NULL) {
    response = response_constructor("HTTP/1.1", NOT_FOUND, "Not found");
  } else {
    char *body = matched_route->route_callback(request);
    response = response_constructor("HTTP/1.1", OK, body);
  }

  // calculate content length
  char body_length[12];
  snprintf(body_length, sizeof(body_length), "%lu", strlen(response.body));
  add_header(&response, "Content-Length", body_length);

  printf("%s %s -> %s\n", request.method, request.target, response.status);

  // send response string
  char response_string[MAX_BUFFER_SIZE];
  response_to_string(response_string, &response);

  if (send(*client_fd, response_string, strlen(response_string), 0) == -1) {
    perror("send content");
    exit(1);
  }
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
      respond(server, &client_fd);
      close(client_fd);
      exit(0);
    }
    close(client_fd); // parent doesnt need this
  }
}
