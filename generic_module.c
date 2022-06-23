#include "generic_module.h"

char *readLineUntilDelimiter(int fd, char delimiter) {
  char *text = (char *)malloc(1);
  char current;
  int i = 0;
  int length = 0;

  while ((length += read(fd, &current, 1)) > 0) {
    text[i] = current;
    text = (char *)realloc(text, ++i + 1);

    if (current == delimiter) {
      break;
    }
  }

  text[i - 1] = '\0';

  return text;
}

char *toLowerCase(char *text) {
  char *lower = calloc(strlen(text) + 1, sizeof(char));

  for (int i = 0; i < strlen(text); i++) {
    lower[i] = tolower(text[i]);
  }

  return lower;
}

int isEmpty(char *text) {
  while (*text != '\0' && *text != '\n') {
    if (!isspace(*text)) {
      return FALSE;
    }

    text++;
  }

  return TRUE;
}

char **split(char *text, char *delimiter) {
  char **split = NULL;
  int index = 0, split_counter = 0;

  // Reserve memory dynamically
  split = (char **)malloc(sizeof(char *));

  // Split text by delimiter for the first appearance of the delimiter
  split[index] = strtok(text, delimiter);

  // Iterate through all the appearances of the delimiter
  while (split[index] != NULL) {
    split_counter++;
    split = realloc(split, (split_counter + 1) * sizeof(char *));
    split[++index] = strtok(NULL, delimiter);
  }

  return split;
}

int countSplits(char **array) {
  int counter = 0;

  while (*array != 0) {
    counter++;
    array++;
  }

  return counter;
}