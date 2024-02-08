#include "response.h"
#include "header.h"
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_SIZE 2048

struct Response response_constructor(char *version, char *status, char *body,
                                     int num_headers, struct Header *headers) {
  struct Response response;
  response.version = version;
  response.status = status;
  response.body = body;
  response.num_headers = num_headers;
  response.headers = headers;
  return response;
}

void response_to_string(char *buf, struct Response *response) {
  char headers[MAX_BUFFER_SIZE];

  for (int i = 0; i < response->num_headers; i++) {
    strcat(headers, response->headers[i].key);
    strcat(headers, ": ");
    strcat(headers, response->headers[i].value);
    strcat(headers, "\r\n");
  }

  snprintf(buf, MAX_BUFFER_SIZE, "%s %s\r\n%s\r\n%s", response->version,
           response->status, headers, response->body);
}
