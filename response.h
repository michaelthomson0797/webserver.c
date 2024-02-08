#ifndef RESPONSE_H
#define RESPONSE_H

#define OK "200 OK"
#define NOT_FOUND "404 Not Found"
#define INTERNAL_SERVER_ERROR "500 Internal Server Error"

struct Response {
  char *version;
  char *status;
  char *body;
  int num_headers;
  struct Header *headers;
};

struct Response response_constructor(char *version, char *status, char *body,
                                     int num_headers, struct Header *headers);

void response_to_string(char *buf, struct Response *response);

#endif // !RESPONSE_H
