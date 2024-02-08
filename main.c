#include "server.h"
#include <stdlib.h>

int main(void) {
  struct Server server = server_constructor("3000");
  launch(&server);
  return EXIT_SUCCESS;
}
