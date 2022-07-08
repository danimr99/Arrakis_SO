#define _GNU_SOURCE
#include "generic_module.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 


#define FRAME_LENGTH 256
#define FRAME_ORIGIN_LENGTH 15 
#define FRAME_TYPE_LENGTH 1 
#define FRAME_DATA_LENGTH 240

#define PHOTO_HASH_LENGTH 33

typedef struct {
  char origin[FRAME_ORIGIN_LENGTH];
  char type;
  char data[FRAME_DATA_LENGTH];
} Frame;

typedef struct {
  char name[30];
  int size;
  char hash[PHOTO_HASH_LENGTH];
  int photo_fd;
} Photo;

#define ORIGIN_FREMEN 1 
#define ORIGIN_ATREIDES 2 

#define LOGIN_TYPE 'C'
#define LOGIN_SUCCESSFUL_TYPE 'O' 
#define LOGIN_ERROR_TYPE 'E' 

#define SEARCH_TYPE 'S'
#define SEARCH_SUCCESSFUL_TYPE 'L'
#define SEARCH_ERROR_TYPE 'K'

#define PHOTO_INFO_TYPE 'F'
#define PHOTO_REQUEST_TYPE 'P'
#define PHOTO_TRANSFER_TYPE 'D'
#define PHOTO_SUCCESSFUL_TYPE 'I'
#define PHOTO_ERROR_TYPE 'R'

#define LOGOUT_TYPE 'Q' 

#define UNKNOWN_TYPE 'Z'

char *initializeFrame(int origin);

char *generateRequestLoginFrame(char *frame, char type, char *name, char *zip_code);

char *generateResponseLoginFrame(char *frame, char type, int id);

char *generateRequestSearchFrame(char *frame, char type, char *username, int user_id, char *zip_code);

char *generateResponseSearchFrame(char *frame, char type, char *data);

int getAtreidesPhotoFD(char *directory, char *photo_name);

char *getPhotoMD5Hash(char *photo_path);

char *generateRequestPhotoFrame(char *frame, char *photo_name);

char *generatePhotoInformationFrame(char *frame, Photo photo);

char *generateInexistentPhotoFrame(char *frame, char *data);

char *generatePhotoFrame(char *frame, char photo_data[FRAME_DATA_LENGTH]);

Photo receivePhotoInformationFrame(char *data);

void processPhotoFrame(int user_id, int socket_fd, char *directory, Photo photo);

void transferPhoto(int origin, int socket_fd, Photo photo);

char *generatePhotoSuccessTransferFrame(char *frame);

char *generatePhotoErrorTransferFrame(char *frame);

char *generateRequestLogoutFrame(char *frame, char type, char *username, int user_id);

char *generateUnknownTypeFrame(char *frame);

void sendFrame(int fd, char *frame);

Frame receiveFrame(int fd);

char *readFromFrameUntilDelimiter(char *array, int index, char delimiter);