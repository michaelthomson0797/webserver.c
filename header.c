#include "header.h"
#include <string.h>

struct Header header_constructor(char *key, char *value) {
  struct Header header;
  header.key = strdup(key);
  header.value = strdup(value);
  return header;
}
