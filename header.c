#include "header.h"

struct Header header_constructor(char *key, char *value) {
  struct Header header;
  header.key = key;
  header.value = value;
  return header;
}
