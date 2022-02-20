#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


#define MAX_LENGTH 100

// Define for printing on terminal
#define printMessage(x) write(1, x, strlen(x))

char * readLineUntilDelimiter(int fd, char delimiter);