#include "response.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 2048

struct Response response_constructor(char *version, char *status, char *body) {
  struct Response response;
  response.version = strdup(version);
  response.status = strdup(status);
  response.body = strdup(body);
  response.num_headers = 0;
  response.headers = NULL;
  return response;
}

void add_header(struct Response *response, char *key, char *value) {
  struct Header header = header_constructor(key, value);
  response->headers = realloc(
      response->headers, sizeof(struct Header) * (response->num_headers + 1));
  response->headers[response->num_headers++] = header;
}

void response_to_string(char *buf, struct Response *response) {
  char headers[MAX_BUFFER_SIZE];
  headers[0] = '\0';

  for (int i = 0; i < response->num_headers; i++) {
    strcat(headers, response->headers[i].key);
    strcat(headers, ": ");
    strcat(headers, response->headers[i].value);
    strcat(headers, "\r\n");
  }


  snprintf(buf, MAX_BUFFER_SIZE, "%s %s\r\n%s\r\n%s", response->version,
           response->status, headers, response->body);
}
