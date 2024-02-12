#include "request.h"
#include "server.h"
#include <stdlib.h>

char *home(struct Request request) {
  return read_file("index.html");
}

char *clicked(struct Request request) {
  return read_file("clicked.html");
}

int main(void) {
  struct Server server = server_constructor("3000");
  add_route(&server, "GET", "/", home);
  add_route(&server, "POST", "/clicked", clicked);
  launch(&server);
  return EXIT_SUCCESS;
}
