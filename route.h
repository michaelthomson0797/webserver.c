#ifndef ROUTE_H
#define ROUTE_H

#include "request.h"

typedef char *(*Route_Callback)(struct Request request);

struct Route {
  char *target;
  char *method;
  Route_Callback route_callback;
};

struct Route route_constructor(char *target, char *method,
                               Route_Callback route_callback);

#endif // !ROUTE_H
