#ifndef HEADER_H
#define HEADER_H

struct Header {
  char *key;
  char *value;
};

struct Header header_constructor(char *key, char *value);

#endif // !HEADER_H
