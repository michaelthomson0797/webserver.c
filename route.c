#include <string.h>

#include "route.h"

struct Route route_constructor(char *target, char *method,
                               Route_Callback route_callback) {
  struct Route route;
  route.target = strdup(target);
  route.method = strdup(method);
  route.route_callback = route_callback;
  return route;
}
