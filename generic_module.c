#include "generic_module.h"

/*
 * Function that retrieves a text until a delimiter.
 * 
 * @param fd File descriptor to read from.
 * @param delimiter Delimiter.
 * @return Text until delimiter.
*/
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

/*
 * Function that converts a text to lower case.
 * 
 * @param text Text to convert.
 * @return Text converted.
*/
char *toLowerCase(char *text) {
  char *lower = calloc(strlen(text) + 1, sizeof(char));

  for (int i = 0; i < strlen(text); i++) {
    lower[i] = tolower(text[i]);
  }

  return lower;
}

/*
 * Function that checks if a string is empty.
 * 
 * @param text Text to check.
 * @return Result of the operation.
*/
int isEmpty(char *text) {
  while (*text != '\0' && *text != '\n') {
    if (!isspace(*text)) {
      return FALSE;
    }

    text++;
  }

  return TRUE;
}

/*
 * Function that splits a text by delimiter.
 * 
 * @param text Text to split.
 * @param delimiter Delimiter.
 * @return Text splitted.
*/
char **split(char *text, char *delimiter) {
  char **split;
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

/*
 * Function that calculates the number of words from a string.
 * 
 * @param array String to check.
 * @return Number of splits.
*/
int countSplits(char **array) {
  int counter = 0;

  while (*array != 0) {
    counter++;
    array++;
  }

  return counter;
}

/*
 * Function that calculates the number of digits of a number.
 * 
 * @param number Number to check.
 * @return Number of digits.
*/
unsigned countDigits(const unsigned number) {
  if (number < 10) return 1;
  return 1 + countDigits(number / 10);
}