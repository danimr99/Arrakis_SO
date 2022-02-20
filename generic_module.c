#include "generic_module.h"

char * readLineUntilDelimiter(int fd, char delimiter) {
  char * text = (char *) malloc(1);
  char current;
  int i = 0;
  int length = 0;

  while ((length += read(fd, & current, 1)) > 0) {
    text[i] = current;
    text = (char *) realloc(text, ++i + 1);

    if(current == delimiter) {
      break;
    }
  }

  text[i - 1] = '\0';

  return text;
}