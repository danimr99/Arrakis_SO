#define _GNU_SOURCE

#include "generic_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char origin[15];
  char type;
  char data[240];
} Frame;

#define FRAME_LENGTH 256
#define FRAME_ORIGIN_LENGTH 15
#define FRAME_TYPE_LENGTH 1
#define FRAME_DATA_LENGTH 240

#define ORIGIN_FREMEN 1
#define ORIGIN_ATREIDES 2

#define LOGIN_TYPE 'C'
#define LOGIN_SUCCESSFUL_TYPE 'O'
#define LOGIN_ERROR_TYPE 'E'
#define LOGOUT_TYPE 'Q'

char *initializeFrame(int origin);

char *generateLoginFrame(char *frame, char type, char *name, char *zip_code);

void sendFrame(int origin, int fd, char *frame);

Frame receiveFrame(int fd);