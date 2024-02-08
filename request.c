#include "request.h"
#include "header.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

#define MAX_HEADER_NUM 128

struct Request request_constructor(char *raw_request) {
  struct Request request;
  request.method = strstrtok(raw_request, " ");
  request.target = strstrtok(NULL, " ");
  request.version = strstrtok(NULL, "\r\n");
  char *raw_headers = strstrtok(NULL, "\r\n\r\n");
  request.body = strstrtok(NULL, "\0");

  int num_headers = 0;
  struct Header headers[MAX_HEADER_NUM];
  char *header_name = strstrtok(raw_headers, ": ");
  if (header_name) {
    char *header_value;
    do {
      header_value = strstrtok(NULL, "\r\n");
      headers[num_headers++] = header_constructor(header_name, header_value);
    } while ((header_name = strstrtok(NULL, ": ")));
  }

  request.num_headers = num_headers;
  request.headers = headers;

  return request;
}
