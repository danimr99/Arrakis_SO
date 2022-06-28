#define _GNU_SOURCE
#include "generic_module.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 


#define FRAME_LENGTH 256
#define FRAME_ORIGIN_LENGTH 15 
#define FRAME_TYPE_LENGTH 1 
#define FRAME_DATA_LENGTH 240

typedef struct {
  char origin[FRAME_ORIGIN_LENGTH];
  char type;
  char data[FRAME_DATA_LENGTH];
} Frame;

#define ORIGIN_FREMEN 1 
#define ORIGIN_ATREIDES 2 

#define LOGIN_TYPE 'C'
#define LOGIN_SUCCESSFUL_TYPE 'O' 
#define LOGIN_ERROR_TYPE 'E' 
#define LOGOUT_TYPE 'Q' 

char *initializeFrame(int origin);

char *generateRequestLoginFrame(char *frame, char type, char *name, char *zip_code);

char *generateResponseLoginFrame(char *frame, char type, int id);

void sendFrame(int origin, int fd, char *frame);

Frame receiveFrame(int fd);

char *readFromFrameUntilDelimiter(char *array, int index, char delimiter);