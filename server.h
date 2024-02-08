#ifndef SERVER_H
#define SERVER_H

#include "header.h"

#define BACKLOG 1000

#define GET "GET";
#define POST "POST";

struct Route {
  char *path;
  char *method;
};

struct Server {
  const char *PORT;
};

struct Server server_constructor(char *PORT);

void launch(struct Server *server);

#endif // !SERVER_H
