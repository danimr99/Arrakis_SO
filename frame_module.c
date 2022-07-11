#include "frame_module.h"

/*
 * Function that fills the remaining positions of an array with '\0'.
 *
 * @param array Array to fill.
 * @param index Index of the starting position to fill.
 * @param size Limit of positions of the array.
 * @return Array filled.
*/
char *fill(char * array, int index, int size) {
  for (int i = index; i < size; i++) {
    array[i] = '\0';
  }

  return array;
}

/*
 * Function that reads from a frame between the index position until a delimiter.
 *
 * @param array Frame.
 * @param index Index of the starting position to read.
 * @param delimiter Delimiter.
 * @return Array from frame until delimiter.
*/
char *readFromFrameUntilDelimiter(char *array, int index, char delimiter) {
  char *text;
  int i = 0;

  // Reserve memory dynamically for the first character
  text = (char *)malloc(sizeof(char));

  // Get all the text until the specified delimiter
  while(array[index] != delimiter && index < FRAME_DATA_LENGTH) {    
    text[i] = array[index];
    text = (char *)realloc(text, i + 2);
    index++;
    i++;
  }

  // Concatenate '\0' at the end of the string
  text[i] = '\0';

  return text;
}

/*
 * Function that initializes a frame with an origin.
 *
 * @param origin Sender of the frame.
 * @return Frame with the origin set up.
*/
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
  }

  // Fill with '\0' the rest of the origin
  frame = fill(frame, strlen(frame), FRAME_ORIGIN_LENGTH);

  return frame;
}

/*
 * Function that checks who is the sender of a frame.
 *
 * @param frame_origin Origin field of a frame.
 * @return Sender of the frame.
*/
int getFrameSender(char frame_origin[FRAME_ORIGIN_LENGTH]) {
  if (strcmp(frame_origin, "FREMEN") == 0) {
    return ORIGIN_FREMEN;
  } else if (strcmp(frame_origin, "ATREIDES") == 0) {
    return ORIGIN_ATREIDES;
  } else {
    printMessage("ERROR: No s'ha pogut determinar l'origen d'una trama rebuda\n");
    return -1;
  }
}

/*
 * Function that generates the request frame to login.
 *
 * @param frame Frame to fill.
 * @param type Type of login frame.
 * @param name Name of the user to login.
 * @param zip_code Zip code of the user to login.
 * @return Login request frame filled.
*/
char *generateRequestLoginFrame(char *frame, char type, char *name, char *zip_code) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = type;

  // Concatenate data
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

/*
 * Function that generates the login response frame.
 *
 * @param frame Frame to fill.
 * @param type Type of login frame.
 * @param id ID of the logged in user.
 * @return Login response frame filled.
*/
char *generateResponseLoginFrame(char *frame, char type, int id) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = type;

  // Concatenate data
  asprintf(&buffer, "%d", id);

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

/*
 * Function that generates the request frame to search users by zip code.
 *
 * @param frame Frame to fill.
 * @param type Type of search frame.
 * @param name Name of the user who executes the search command from Fremen.
 * @param user_id ID of the user who executes the search command from Fremen.
 * @param zip_code Zip code to search.
 * @return Search request frame filled.
*/
char *generateRequestSearchFrame(char *frame, char type, char *username, int user_id, char *zip_code) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = type;

  // Concatenate data
  asprintf(&buffer, "%s*%d*%s", username, user_id, zip_code);

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

/*
 * Function that generates the search response frame.
 *
 * @param frame Frame to fill.
 * @param type Type of search frame.
 * @param name Data sent as response from Atreides to a search command.
 * @return Search frame filled.
*/
char *generateResponseSearchFrame(char *frame, char type, char *data) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = type;

  // Concatenate data
  asprintf(&buffer, "%s", data);

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

/*
 * Function that retrieves the photo file descriptor on Atreides.
 *
 * @param directory Directory from the Atreides configuration file.
 * @param photo_name ID of the user owner of the photo.
 * @return Photo file descriptor.
*/
int getAtreidesPhotoFD(char *directory, char *photo_name) {
  char *destination_path;
  int destination_fd;

  // Concatenate the directory and the name of the photo
  asprintf(&destination_path, "%s/%s.jpg", directory, photo_name);

  // Open file descriptor of the destination path
  destination_fd = open(destination_path, O_RDONLY);
  free(destination_path);

  return destination_fd;
}

/*
 * Function that generates the MD5SUM hash of a photo.
 *
 * @param photo_path Path where is located the photo.
 * @return MD5SUM hash of the specified photo.
*/
char *getPhotoMD5Hash(char *photo_path) {
  int pipe_fd[2];
  pid_t pid;
  char *hash;

  // Create a pipe and check result
  if (pipe(pipe_fd) == -1) {
    printMessage("ERROR: No s'ha pogut crear una pipe per a comunicar-se entre processos\n");
  }

  // Execute md5sum command on a child and get its PID
  if ((pid = fork()) == 0) {
    // Code to be executed by the child
    // Set STDOUT_FILENO as the file descriptor to use
    dup2(pipe_fd[1], STDOUT_FILENO);

    // Close unused write ends
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // Run md5sum command
    execl("/bin/md5sum", "md5sum", photo_path, NULL);
  } else {
    // Code to be executed by the parent
    // Wait until child has terminated its execution
    waitpid(pid, NULL, 0);

    // Close unused write end
    close(pipe_fd[1]);

    // Reserve memory dynamically for the md5 hash
    hash = (char *)malloc(256 * sizeof(char));

    // Get the hash from the output of the command executed on the child process
    read(pipe_fd[0], hash, 256);
    strtok(hash, " ");

    // Close file descriptors
    close(pipe_fd[0]);

    return hash;
  }

  return NULL;
}

/*
 * Function that generates the photo request frame.
 *
 * @param frame Frame to fill.
 * @param photo_name Name of the requested photo.
 * @return Photo frame filled.
*/
char *generateRequestPhotoFrame(char *frame, char *photo_name) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = PHOTO_REQUEST_TYPE;

  // Concatenate data 
  asprintf(&buffer, "%s", photo_name);

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

/*
 * Function that generates the photo information frame.
 *
 * @param frame Frame to fill.
 * @param photo All photo information.
 * @return Information photo frame filled.
*/
char *generatePhotoInformationFrame(char *frame, Photo photo) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = PHOTO_INFO_TYPE;

  if (photo.photo_fd > 0) {
    // Concatenate data
    asprintf(&buffer, "%s*%d*%s", photo.name, photo.size, photo.hash);
  } else {
    // Set buffer to empty
    buffer = (char *)malloc(sizeof(char));
    buffer[0] = '\0';
  }

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

/*
 * Function that generates an inexistent photo frame.
 *
 * @param frame Frame to fill.
 * @param data Data to be sent.
 * @return Inexistent photo frame filled.
*/
char *generateInexistentPhotoFrame(char *frame, char *data) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = PHOTO_INFO_TYPE;

  // Concatenate data 
  asprintf(&buffer, "%s", data);

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

/*
 * Function that generates a photo frame.
 *
 * @param frame Frame to fill.
 * @param photo_data Data of the photo to be transferred.
 * @return Photo frame filled.
*/
char *generatePhotoFrame(char *frame, char photo_data[FRAME_DATA_LENGTH]) {
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = PHOTO_TRANSFER_TYPE;

  // Get the length of the frame
  frame_length = FRAME_ORIGIN_LENGTH + FRAME_TYPE_LENGTH;

  // Add data to frame
  for (i = frame_length; i < FRAME_LENGTH; i++) {
    frame[i] = photo_data[i - frame_length];
  }

  return frame;
}

/*
 * Function that receives a photo information frame.
 *
 * @param data Data from a photo information frame.
 * @return All the information received from a photo.
*/
Photo receivePhotoInformationFrame(char *data) {
  Photo photo;
  char *buffer;
  int index = 0;
  
  // Get name of the photo
  buffer = readFromFrameUntilDelimiter(data, index, '*');
  strcpy(photo.name, buffer);
  free(buffer);

  // Move index
  index += strlen(photo.name) + 1;

  // Get the size of the photo
  buffer = readFromFrameUntilDelimiter(data, index, '*');
  photo.size = atoi(buffer);
  free(buffer);

  // Move index
  index += countDigits(photo.size) + 1;

  // Get the md5sum hash of the photo
  buffer = readFromFrameUntilDelimiter(data, index, '\0');
  strcpy(photo.hash, buffer);
  free(buffer);

  return photo;
}

/*
 * Function that generates a success photo transfer frame.
 *
 * @param frame Frame to fill.
 * @return Success photo transfer frame filled.
*/
char *generatePhotoSuccessTransferFrame(char *frame) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = PHOTO_SUCCESSFUL_TYPE;

  // Concatenate data
  asprintf(&buffer, "%s", "IMAGE OK");

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

/*
 * Function that generates an error photo transfer frame.
 *
 * @param frame Frame to fill.
 * @return Error photo transfer frame filled.
*/
char *generatePhotoErrorTransferFrame(char *frame) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = PHOTO_ERROR_TYPE;

  // Concatenate data
  asprintf(&buffer, "%s", "IMAGE KO");

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

/*
 * Function that receives a photo frame by frame and processes it properly
 * depending on the sender.
 *
 * @param user_id ID of the user that sent/requested a photo.
 * @param socket_fd Socket to receive from the photo frame by frame.
 * @param directory Directory to store the photo.
 * @param photo Information of the photo to be received and processed.
*/
void processPhotoFrame(int user_id, int socket_fd, char *directory, Photo photo) {
  Frame received_frame;
  char *file_name, *destination_path, text[MAX_LENGTH], transferred_photo_hash[PHOTO_HASH_LENGTH],
    *response_frame, *buffer;
  int destination_fd, number_frames = 0, remainder_data = 0, processed_frames = 0, frame_origin, frame_destination;

  // Concatenate destination directory with the renamed image
  asprintf(&file_name, "%d.jpg", user_id);
  asprintf(&destination_path, "%s/%s", directory, file_name);

  // Get the number of frames to receive for completing the photo transfer
  number_frames = photo.size / FRAME_DATA_LENGTH;

  // Check if data does not fulfill the last frame (exists remainder from the division above)
  if ((remainder_data = photo.size % FRAME_DATA_LENGTH) != 0) {
    number_frames++;
  }

  // Open the file descriptor of the photo on the destination directory
  destination_fd = open(destination_path, O_CREAT | O_WRONLY | O_TRUNC, 0666);

  while (processed_frames < number_frames) {
    // Receive frame
    received_frame = receiveFrame(socket_fd);

    // Check if it is the last frame to be received
    if (processed_frames == (number_frames - 1) && remainder_data != 0) {
      write(destination_fd, received_frame.data, remainder_data);
    } else {
      write(destination_fd, received_frame.data, FRAME_DATA_LENGTH);
    }

    processed_frames++;
  }

  // Close photo destination file descriptor
  close(destination_fd);

  // Generate md5sum hash for the transferred if exists photo sent
  buffer = getPhotoMD5Hash(destination_path);
  strcpy(transferred_photo_hash, buffer);
  free(buffer);

  // Get the sender of the photo transferred
  frame_origin = getFrameSender(received_frame.origin);

  // Set the receiver of the photo transferred as the sender of the response frames
  if (frame_origin == ORIGIN_FREMEN) {
    frame_destination = ORIGIN_ATREIDES;
  } else if (frame_origin == ORIGIN_ATREIDES) {
    frame_destination = ORIGIN_FREMEN;
  }

  // Initialize the response frame
  response_frame = initializeFrame(frame_destination);

  // Check photo integrity after transfer
  if (strcmp(transferred_photo_hash, photo.hash) == 0) {
    // Generate photo success frame
    response_frame = generatePhotoSuccessTransferFrame(response_frame);

    // Check the receiver of the photo 
    if (frame_destination == ORIGIN_ATREIDES) {
      // Print message informing that the photo was saved with the new name
      sprintf(text, "Guardada com %s\n", file_name);
      printMessage(text);
    } else if(frame_destination == ORIGIN_FREMEN) {
      // Print message informing that the photo was downloaded
      printMessage("Foto descarregada\n");
    }
  } else {
    // Generate photo error frame
    response_frame = generatePhotoErrorTransferFrame(response_frame);

    // Print message informing that the photo was not identical (hash integrity)
    sprintf(text, "ERROR: El hash md5sum de la foto %s no és correcte\n", file_name);
    printMessage(text);
  }

  // Send photo response
  sendFrame(socket_fd, response_frame);

  // Free up memory
  free(file_name);
  free(destination_path);
  free(response_frame);
}

/*
 * Function that sends a photo frame by frame.
 *
 * @param origin Sender of the photo.
 * @param socket_fd Socket to send the photo frame by frame.
 * @param photo Information of the photo to be sent.
*/
void transferPhoto(int origin, int socket_fd, Photo photo) {
  int number_frames = 0, remainder_data = 0, processed_frames = 0;
  char photo_data[FRAME_DATA_LENGTH], *send_frame;

  // Get the number of frames to receive for completing the photo transfer
  number_frames = photo.size / FRAME_DATA_LENGTH;

  // Check if data does not fulfill the last frame (exists remainder from the division above)
  if ((remainder_data = photo.size % FRAME_DATA_LENGTH) != 0) {
    number_frames++;
  }

  // Send required number of frames 
  while (processed_frames < number_frames) {
    // Reset values
    memset(photo_data, 0, sizeof(photo_data));

    // Read data from the photo
    read(photo.photo_fd, photo_data, FRAME_DATA_LENGTH);

    // Initialize and send the photo data frame
    send_frame = initializeFrame(origin);
    send_frame = generatePhotoFrame(send_frame, photo_data);
    sendFrame(socket_fd, send_frame);

    // Increment the number of processed frames
    processed_frames++;

    // Free up memory
    free(send_frame);
    
    // Add a delay between frames
    usleep(300);
  }

  // Close the file descriptor of the photo
  close(photo.photo_fd);  
}

/*
 * Function that generates a request logout frame.
 *
 * @param frame Frame to fill.
 * @param type Type of logout frame.
 * @param username Name of the user who requested to logout.
 * @param user_id ID of the user who requested to logout.
 * @return Logout frame filled.
*/
char *generateRequestLogoutFrame(char *frame, char type, char *username, int user_id) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = type;

  // Concatenate data
  asprintf(&buffer, "%s*%d", username, user_id);

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

/*
 * Function that generates an unknown type frame.
 *
 * @param frame Frame to fill.
 * @return Unknown type frame filled.
*/
char *generateUnknownTypeFrame(char *frame) {
  char *buffer;
  int i, frame_length;

  // Add frame type
  frame[FRAME_ORIGIN_LENGTH] = UNKNOWN_TYPE;

  // Concatenate data
  asprintf(&buffer, "%s", "ERROR T");

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

/*
 * Function that sends a frame.
 *
 * @param fd Socket to send a frame to.
 * @param frame Frame to send.
*/
void sendFrame(int fd, char *frame) {
  write(fd, frame, FRAME_LENGTH);
}

/*
 * Function that receives a frame.
 *
 * @param fd Socket to receive a frame from.
 * @return Frame received.
*/
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