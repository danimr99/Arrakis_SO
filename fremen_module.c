#include "fremen_module.h"

// List of custom commands
const char *custom_commands[] = {"login", "search", "send", "photo", "logout"};

// Global variables
extern FremenConfiguration fremen_configuration;
char *username;
int user_id, socket_fd = 0;
DownloadedPhotosList downloaded_photos_list;

/*
 * Function that retrieves the Fremen configuration from the configuration file specified.
 *
 * @param config_file_fd File descriptor of the configuration file.
 * @return Configuration of Fremen.
*/
FremenConfiguration getFremenConfiguration(int config_file_fd) {
  FremenConfiguration fremen_configuration;
  char *buffer;

  // Get clean time period from Fremen configuration file
  buffer = readLineUntilDelimiter(config_file_fd, '\n');
  fremen_configuration.clean_time = atoi(buffer);
  free(buffer);

  // Get IP from Fremen configuration file
  fremen_configuration.ip = readLineUntilDelimiter(config_file_fd, '\n');

  // Get directory from Fremen configuration file
  buffer = readLineUntilDelimiter(config_file_fd, '\n');
  fremen_configuration.port = atoi(buffer);
  free(buffer);

  // Get directory from Fremen configuration file
  buffer = readLineUntilDelimiter(config_file_fd, '\n');
  fremen_configuration.directory = (char *)malloc(sizeof(char) * strlen(buffer));
  memset(fremen_configuration.directory, 0, strlen(buffer) * sizeof(char));

  // Iterate through the string skipping the first character ('/')
  for (int i = 1; buffer[i] != '\0'; i++) {
    fremen_configuration.directory[i - 1] = buffer[i];
  }
  
  free(buffer);

  // Check that exists a directory name on the Fremen configuration
  if (!isEmpty(fremen_configuration.directory)) {
    // Create configuration directory if it does not exist
    struct stat stats = {0};

    if (stat(fremen_configuration.directory, &stats) == -1) {
      mkdir(fremen_configuration.directory, 0700);
    }
  }
  
  // Close configuration file descriptor
  close(config_file_fd);

  return fremen_configuration;
}

/*
 * Function that checks if a photo was already downloaded by a Fremen client from Atreides.
 *
 * @param photo_name Name of the photo.
 * @return Result of the operation.
*/
int existsDownloadedPhoto(char *photo_name) {
  // Iterate through the list of downloaded photos
  for (int i = 0; i < downloaded_photos_list.photos_quantity; i++) {
    // Check if downloaded photos list already contains the photo name
    if (strcmp(downloaded_photos_list.photo_names[i], photo_name) == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/*
 * Function that adds a photo to the list of downloaded photos.
 *
 * @param photo_name Name of the photo.
*/
void addDownloadedPhotoToList(char *photo_name) {
  // Increase counter of downloaded photos
  downloaded_photos_list.photos_quantity++;

  // Check if is the first photo downloaded
  if (downloaded_photos_list.photos_quantity == 1) {
    // Reserve memory dynamically for the list of photo names
    downloaded_photos_list.photo_names = (char **)malloc(downloaded_photos_list.photos_quantity * sizeof(downloaded_photos_list.photo_names));
  } else {
    // Reserve memory dynamically for the list of photo names
    downloaded_photos_list.photo_names = (char **)realloc(downloaded_photos_list.photo_names, 
      downloaded_photos_list.photos_quantity * sizeof(downloaded_photos_list.photo_names));
  }

  // Reserve memory dynamically for the photo name to be added
  downloaded_photos_list.photo_names[downloaded_photos_list.photos_quantity - 1] = (char *)malloc((strlen(photo_name) + 1) * sizeof(char));

  // Assign values to the list
  strcpy(downloaded_photos_list.photo_names[downloaded_photos_list.photos_quantity - 1], photo_name);
}

/*
 * Function that deletes all the list of downloaded photos from the Fremen directory.
*/
void deleteDownloadedPhotos() {
  char *downloaded_photo_path, *buffer, **command;

  // Iterate through the list of downloaded photos
  for (int i = 0; i < downloaded_photos_list.photos_quantity; i++) {
    // Concatenate the directory and the name of the photo
    asprintf(&downloaded_photo_path, "%s/%s", fremen_configuration.directory, downloaded_photos_list.photo_names[i]);

    // Concatenate rm command
    asprintf(&buffer, "rm %s", downloaded_photo_path);

    // Split concatenated command
    command = split(buffer, " ");

    // Run rm command
    runLinuxCommand(command);

    // Free memory for the photo name deleted
    free(downloaded_photos_list.photo_names[i]);
    free(downloaded_photo_path);
    free(buffer);
    free(command);
  }

  // Reset values for the list of downloaded photos
  downloaded_photos_list.photos_quantity = 0;
  free(downloaded_photos_list.photo_names);
}

/*
 * Function that converts the command introduced by a user to lower case. Only converts
 * the first word.
 *
 * @param command Command introduced by the user.
 * @return Converted command.
*/
char **convertCommandToLowerCase(char **command) {
  char *buffer;

  for(int i = 0; i < 1; i++) {
    buffer = toLowerCase(command[i]);
    strcpy(command[i], buffer);
    free(buffer);
  }

  return command;
}

/*
 * Function that executes Linux commands.
 *
 * @param command Command to be executed.
*/
void runLinuxCommand(char **command) {
  pid_t pid;

  // Execute Linux command on a child and get its PID
  if ((pid = fork()) == 0) {
    // Code to be executed by the child
    // Execute Linux command and get the result of the operation
    int status_code = execvp(command[0], command);

    // Check if execution of the Linux command has been an error
    if (status_code == -1) {
      printMessage("ERROR: Comanda desconeguda\n");
    }

    // Exit child process
    exit(0);
  } else {
    // Code to be executed by the parent
    // Wait until child has terminated its execution
    waitpid(pid, NULL, 0);
  }
}

/*
 * Function that configures the socket connection to Atreides server.
 *
 * @param fremen_configuration Fremen configuration.
 * @return Socket file descriptor.
*/
int configureSocket(FremenConfiguration fremen_configuration) {
  struct sockaddr_in server;
  int socket_fd;  

  // Open socket and check if it has been successfully created
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printMessage("ERROR: No s'ha pogut crear socket de connexió\n");

    // Mark socket as invalid
    return -1;
  }

  // Set up socket
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(fremen_configuration.port);

  // Configure Atreides server IP
  if (inet_pton(AF_INET, fremen_configuration.ip, &server.sin_addr) < 0) {
    printMessage("ERROR: No s'ha pogut configurar la IP del servidor\n");

    // Mark socket as invalid
    return -1;
  }

  // Connect socket to Atreides server
  if (connect(socket_fd, (void *) &server, sizeof(server)) < 0) {
    printMessage("ERROR: No s'ha pogut connectar a Atreides\n");

    // Mark socket as invalid
    return -1;
  }

  return socket_fd;
}

/*
 * Function that receives the results of a search command from Atreides and displays
 * the results.
 *
 * @param socket_fd Socket to read the Atreides search response frame from.
 * @param zip_code Requested zip code.
*/
void showSearchResults(int socket_fd, char *zip_code) {
  Frame frame;
  char *buffer, text[MAX_LENGTH];
  UsersList list;
  int index = 0, users_processed_counter = 0;

  // Receive response
  frame = receiveFrame(socket_fd);

  // Set default value to the number of results found on the list
  list.users_quantity = 0;

  // Set default values to text
  memset(text, 0, sizeof(text));

  // Get the number of users found matching the zip code
  buffer = readFromFrameUntilDelimiter(frame.data, 0, '*');
  list.users_quantity = atoi(buffer);
  free(buffer);

  // Check if there are results found
  if (list.users_quantity > 0) {
    // Reserve memory dynamically for the amount of results
    list.users = (User *)calloc(list.users_quantity, sizeof(User));

    // Set the start index for getting the information from the frame
    index = countDigits(list.users_quantity) + 1;
    
    // Process all the data from the received frame
    while (frame.data[index] != '\0' && index < FRAME_DATA_LENGTH) {
      // Get username
      list.users[users_processed_counter].username = readFromFrameUntilDelimiter(frame.data, index, '*');

      // Move the index
      index += strlen(list.users[users_processed_counter].username) + 1;

      // Get user ID
      if (users_processed_counter == (list.users_quantity - 1)) {
        buffer = readFromFrameUntilDelimiter(frame.data, index, '\0');
      } else {
        buffer = readFromFrameUntilDelimiter(frame.data, index, '*');
      }
      
      list.users[users_processed_counter].id = atoi(buffer);
      free(buffer);

      // Move the index
      index += countDigits(list.users[users_processed_counter].id) + 1;

      // Increase the number of processed users results
      users_processed_counter++;
    }

    // Check if all results have been received
    if (users_processed_counter < list.users_quantity) {
      // Keep receiving frames with the data remaining
      while (users_processed_counter < list.users_quantity) {
        // Receive another response
        frame = receiveFrame(socket_fd);

        // Reset index on read another frame
        index = 0;

        // Process all the data from the received frame
        while (frame.data[index] != '\0' && index < FRAME_DATA_LENGTH) {
          // Get username
          list.users[users_processed_counter].username = readFromFrameUntilDelimiter(frame.data, index, '*');

          // Move the index
          index += strlen(list.users[users_processed_counter].username) + 1;

          // Get user ID
          if (users_processed_counter == (list.users_quantity - 1)) {
            buffer = readFromFrameUntilDelimiter(frame.data, index, '\0');
          } else {
            buffer = readFromFrameUntilDelimiter(frame.data, index, '*');
          }
          
          list.users[users_processed_counter].id = atoi(buffer);
          free(buffer);

          // Move the index
          index += countDigits(list.users[users_processed_counter].id) + 1;

          // Increase the number of processed users results
          users_processed_counter++;
        }
      }
    }

    // Print users list
    if (list.users_quantity == 0 || list.users_quantity > 1) {
      sprintf(text, "Feta la cerca\nHi ha %d persones humanes a %s\n", list.users_quantity, zip_code);
    } else {
      sprintf(text, "Feta la cerca\nHi ha %d persona humana a %s\n", list.users_quantity, zip_code);
    }
    printMessage(text);

    // Print each result
    for (int i = 0; i < list.users_quantity; i++) {
      sprintf(text, "%d %s\n", list.users[i].id, list.users[i].username);
      printMessage(text);
    }

    // Free memory of the list
    for (int i = 0; i < list.users_quantity; i++) {
      free(list.users[i].username);
    }

    free(list.users);
  } else {
    sprintf(text, "No hi ha persones humanes a %s\n", zip_code);
    printMessage(text);
  }
}

/*
 * Function that retrieves the information of a photo.
 *
 * @param photo_name Name of the photo.
 * @return Information of the photo.
*/
Photo getPhotoInformation(char *photo_name) {
  char *buffer;
  Photo photo;
  char *photo_path;

  // Set default values to photo
  memset(photo.name, 0, sizeof(photo.name));
  memset(photo.hash, 0, sizeof(photo.hash));

  // Set photo name
  strcpy(photo.name, photo_name);

  // Get the image path
  asprintf(&photo_path, "%s/%s", fremen_configuration.directory, photo.name);

  // Get the photo file descriptor
  photo.photo_fd = open(photo_path, O_RDONLY);

  // Check if photo exists
  if (photo.photo_fd > 0) {
    // Get the MD5 hash of the photo
    buffer = getPhotoMD5Hash(photo_path);
    strcpy(photo.hash, buffer);
    free(buffer);

    // Get the size of the photo
    struct stat stats;
    if (stat(photo_path, &stats) == 0) {
      photo.size = stats.st_size;
    }
  } else {
    printMessage("ERROR: No s'ha trobat la imatge o no existeix\n");
  }

  free(photo_path);

  return photo;
}

/*
 * Function that simulates the bash shell.
 *
 * @param fremen_configuration Fremen configuration.
*/
void simulateBashShell(FremenConfiguration fremen_configuration) {
  char *buffer, **command, *frame, text[MAX_LENGTH];
  int args_counter = 0, received_photo_id;
  Frame received_frame;
  Photo photo;

  printMessage("Benvingut a Fremen");

  // Set default number of downloaded photos
  downloaded_photos_list.photos_quantity = 0;

  while(TRUE) {
    printMessage("\n$ ");

    // Get command introduced by the user
    buffer = readLineUntilDelimiter(STDIN_FILENO, '\n');

    // Check if command is not an empty string
    if (!isEmpty(buffer)) {
      // Split command by spaces and get the number of arguments
      command = split(buffer, " ");
      args_counter = countSplits(command) - 1;

      // Format command to lower case
      command = convertCommandToLowerCase(command);

      // Check if command matches with a custom or Linux command
      if (strcmp(command[0], custom_commands[0]) == 0) {
        if (args_counter > LOGIN_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda LOGIN\n");
        } else if (args_counter < LOGIN_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda LOGIN\n");
        } else {
          // Connect to Atreides (only available once)
          if (socket_fd == 0) {
            // Configure socket connection to Atreides
            socket_fd = configureSocket(fremen_configuration);

            // Check if Atreides server is running
            if (socket_fd > 0) {
              // Generate login frame
              frame = initializeFrame(ORIGIN_FREMEN);
              frame = generateRequestLoginFrame(frame, LOGIN_TYPE, command[1], command[2]);

              // Send login frame to Atreides
              sendFrame(socket_fd, frame);
              free(frame);

              // Receive response from Atreides
              received_frame = receiveFrame(socket_fd);

              // Check type response received
              if (received_frame.type == LOGIN_SUCCESSFUL_TYPE) {
                // Get username
                username = (char *) malloc((sizeof(char) * strlen(command[1])) + 1);
                strcpy(username, command[1]);

                // Get user ID from the response
                user_id = atoi(received_frame.data);
                
                sprintf(text, "Benvingut %s. Tens ID %d.\nAra estàs connectat a Atreides\n", username, user_id);
                printMessage(text);
              } else if (received_frame.type == LOGIN_ERROR_TYPE) {
                printMessage("ERROR: No s'ha pogut fer login\n");
              } else {
                printMessage("ERROR: Ha ocurregut un error desconegut\n");
              }
            } else {
              printMessage("ERROR: Atreides no està funcionant\n");

              // Set socket to default value to allow retry connection
              socket_fd = 0;
            }
          } else if (socket_fd < 0) {
            printMessage("ERROR: Atreides no està funcionant\n");

            // Set socket to default value to allow retry connection
            socket_fd = 0;
          } else {
            printMessage("ERROR: Ja estàs connectat a Atreides\n");
          }
        }
      } else if (strcmp(command[0], custom_commands[1]) == 0) {
        if (args_counter > SEARCH_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda SEARCH\n");
        } else if (args_counter < SEARCH_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda SEARCH\n");
        } else {
          // Check that user has connected to Atreides previously
          if (socket_fd > 0) {
            // Generate search frame
            frame = initializeFrame(ORIGIN_FREMEN);
            frame = generateRequestSearchFrame(frame, SEARCH_TYPE, username, user_id, command[1]);

            // Send search frame to Atreides
            sendFrame(socket_fd, frame);
            free(frame);

            // Get response and show results
            showSearchResults(socket_fd, command[1]);
          } else {
            printMessage("ERROR: No estàs connectat a Atreides encara. Prova de fer login\n");
          }
        }
      } else if (strcmp(command[0], custom_commands[2]) == 0) {
        if (args_counter > SEND_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda SEND\n");
        } else if (args_counter < SEND_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda SEND\n");
        } else {
          // Check that user has connected to Atreides previously
          if (socket_fd > 0) {
            // Generate and send photo information frame
            frame = initializeFrame(ORIGIN_FREMEN);
            photo = getPhotoInformation(command[1]);
            frame = generatePhotoInformationFrame(frame, photo);
            sendFrame(socket_fd, frame);
            free(frame);

            // Check if exists the photo introduced to be sent
            if (photo.photo_fd > 0) {
              // Send photo to Atreides using frames
              transferPhoto(ORIGIN_FREMEN, socket_fd, photo);
              close(photo.photo_fd);

              // Read response
              received_frame = receiveFrame(socket_fd);

              // Receive response result of the photo transfer from Atreides (success or error)
              if (received_frame.type == PHOTO_SUCCESSFUL_TYPE) {
                printMessage("Foto enviada amb èxit a Atreides\n");
              } else if (received_frame.type == PHOTO_ERROR_TYPE) {
                printMessage("ERROR: No s'ha pogut enviar correctament la foto a Atreides\n");
              }
            }
          } else {
            printMessage("ERROR: No estàs connectat a Atreides encara. Prova de fer login\n");
          }
        }
      } else if (strcmp(command[0], custom_commands[3]) == 0) {
        if (args_counter > PHOTO_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda PHOTO\n");
        } else if (args_counter < PHOTO_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda PHOTO\n");
        } else {
          // Check that user has connected to Atreides previously
          if (socket_fd > 0) {
            // Generate and send photo frame
            frame = initializeFrame(ORIGIN_FREMEN);
            frame = generateRequestPhotoFrame(frame, command[1]);
            sendFrame(socket_fd, frame);
            free(frame);

            // Get response from Atreides
            received_frame = receiveFrame(socket_fd);

            // Check if Atreides has found the specified photo
            if (strcmp(received_frame.data, "FILE NOT FOUND") == 0) {
              printMessage("ERROR: No hi ha foto registrada\n");
            } else {
              // Get information of the photo that is going to be received
              photo = receivePhotoInformationFrame(received_frame.data);

              // Check if photo was already downloaded by the Fremen client
              if (!existsDownloadedPhoto(photo.name)) {
                // Add downloaded photo to the list
                addDownloadedPhotoToList(photo.name);
              }
              
              // Receive photo transfer from Atreides using frames and response with the result
              received_photo_id = atoi(command[1]);
              processPhotoFrame(received_photo_id, socket_fd, fremen_configuration.directory, photo);
            }
          } else {
            printMessage("ERROR: No estàs connectat a Atreides encara. Prova de fer login\n");
          }
        }
      } else if (strcmp(command[0], custom_commands[4]) == 0) {
        if (args_counter > LOGOUT_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda LOGOUT\n");
        } else if (args_counter < LOGOUT_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda LOGOUT\n");
        } else {
          // Close socket connection if exists
          if (socket_fd > 0) {
            // Generate logout frame
            frame = initializeFrame(ORIGIN_FREMEN);
            frame = generateRequestLogoutFrame(frame, LOGOUT_TYPE, username, user_id);

            // Send logout frame
            sendFrame(socket_fd, frame);
            free(frame);

            // Free up memory of the username
            free(username);

            // Close socket connection
            close(socket_fd);
            socket_fd = 0;

            printMessage("T'has desconnectat d'Atreides\n");
          } else {
            printMessage("ERROR: No estàs connectat a Atreides\n");
          }
        }
      } else {
        // Execute Linux command
        runLinuxCommand(command);
      }

      // Free command
      free(command);
    }

    // Free buffer
    free(buffer);
  }
}