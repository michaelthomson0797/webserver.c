#ifndef SERVER_H
#define SERVER_H

#include "header.h"
#include "route.h"

#define BACKLOG 128

struct Server {
  const char *PORT;
  struct Route *routes;
  int num_routes;
};

struct Server server_constructor(const char *PORT);

void add_route(struct Server *server, char *method, char *target,
               Route_Callback route_callback);

void launch(struct Server *server);

char *read_file(char *filename);

#endif // !SERVER_H
