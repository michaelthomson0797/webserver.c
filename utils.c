#include <string.h>

char *strstrtok(char *input, char *delim) {
  static char *p;

  if (input != NULL) {
    p = input;
  }

  if (p == NULL) {
    return p;
  }

  char *end = strstr(p, delim);
  if (end == NULL) {
    char *temp = p;
    p = NULL;
    return temp;
  }

  char *temp = p;
  *end = '\0';
  p = end + strlen(delim);
  return temp;
}
