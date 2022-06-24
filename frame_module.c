#include "frame_module.h"

char *fill(char * array, int index, int size) {
  for (int i = index; i < size; i++) {
    array[i] = '\0';
  }

  return array;
}

char *initializeFrame(int origin) {
  char *frame;

  // Reserve memory dynamically for a frame 
  frame = (char *)malloc(sizeof(char) * FRAME_LENGTH);

  // Initialize it to default value (0)
  memset(frame, 0, FRAME_LENGTH * sizeof(char));

  // Check the origin introduced
  if(origin == ORIGIN_FREMEN) {
    sprintf(frame, "FREMEN");
  } else if (origin == ORIGIN_ATREIDES) {
    sprintf(frame, "ATREIDES");
  } else {
    printMessage("ERROR: S'ha intentat enviar un frame des d'un origen desconegut\n");
    raise(SIGINT);
  }

  // Fill with '\0' the rest of the origin
  frame = fill(frame, strlen(frame), FRAME_ORIGIN_LENGTH);

  return frame;
}

char *generateLoginFrame(char *frame, char type, char *name, char *zip_code) {
  char * buffer = NULL;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = type;

  // Concatenate data (name and zip code)
  asprintf(&buffer, "%s*%s", name, zip_code);

  // Get the length of the frame
  frame_length = FRAME_ORIGIN_LENGTH + FRAME_TYPE_LENGTH;

  // Add buffer to frame
  for (i = frame_length; buffer[i - frame_length] != '\0'; i++) {
    frame[i] = buffer[i - frame_length];
  }

  // Fill with '\0' the rest of the data
  frame = fill(frame, i, FRAME_DATA_LENGTH);

  // Free buffer
  free(buffer);

  return frame;
}

void sendFrame(int origin, int fd, char *frame) {
  write(fd, frame, FRAME_LENGTH);

  // Check the origin introduced
  if (origin == ORIGIN_ATREIDES) {
    printMessage("Resposta enviada\n");
  }
}

Frame receiveFrame(int fd) {
  unsigned char received_frame[FRAME_LENGTH];
  Frame frame;
  int i = 0;

  // Initialize frame to default value (0)
  memset(frame.origin, 0, sizeof(frame.origin));
  memset(frame.data, 0, sizeof(frame.data));

  // Read income frame
  read(fd, received_frame, sizeof(char) * FRAME_LENGTH);

  // Get the origin from the received frame
  while (i < FRAME_ORIGIN_LENGTH) {
    frame.origin[i] = received_frame[i];
    i++;
  }

  // Get the type from the received frame
  frame.type = received_frame[FRAME_ORIGIN_LENGTH];

  // Get the data from the received frame
  i = FRAME_ORIGIN_LENGTH + FRAME_TYPE_LENGTH;

  while (i < FRAME_LENGTH) {
    frame.data[i - (FRAME_ORIGIN_LENGTH + FRAME_TYPE_LENGTH)] = received_frame[i];
    i++;
  }

  return frame;
}