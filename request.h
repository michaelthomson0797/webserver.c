#ifndef REQUEST_H
#define REQUEST_H

#include "header.h"

struct Request {
  char *method;  // GET, POST, ...
  char *target;  // /, /blog, /blog/post1, ...
  char *version; // HTTP/1.1
  char *body;
  int num_headers;
  struct Header *headers;
};

struct Request request_constructor(char *raw_request);

#endif // !REQUEST_H
