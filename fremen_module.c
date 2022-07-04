#include "fremen_module.h"

// List of custom commands
const char *custom_commands[] = {"login", "search", "send", "photo", "logout"};

// Global variables
char *username = NULL;
int user_id, socket_fd = 0;

FremenConfiguration getFremenConfiguration(int config_file_fd) {
  FremenConfiguration fremen_configuration;
  char *buffer = NULL;

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

  // Iterate through the string skipping the first character ('/')
  for (int i = 1; buffer[i] != '\0'; i++) {
    fremen_configuration.directory[i - 1] = buffer[i];
  }
  
  free(buffer);

  // Create configuration directory if it does not exist
  struct stat st = {0};

  if (stat(fremen_configuration.directory, &st) == -1) {
    mkdir(fremen_configuration.directory, 0700);
  }

  // Close configuration file descriptor
  close(config_file_fd);

  return fremen_configuration;
}

char **convertCommandToLowerCase(char **command) {
  for(int i = 0; i < 1; i++) {
    command[i] = toLowerCase(command[i]);
  }

  return command;
}

void runLinuxCommand(char **command) {
  int child_pid;

  // Execute Linux command on a child and get its PID
  if ((child_pid = fork()) == 0) {
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
    waitpid(child_pid, NULL, 0);
  }
}

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

void showSearchResults(int socket_fd, char *zip_code) {
  Frame frame;
  char *buffer = NULL, text[MAX_LENGTH];
  UsersList list;
  int index, users_processed_counter = 0;

  // Receive response 
  frame = receiveFrame(socket_fd);

  // Get the number of users found matching the zip code
  buffer = readFromFrameUntilDelimiter(frame.data, 0, '*');
  list.users_quantity = atoi(buffer);
  free(buffer);

  // Check if there are results found
  if (list.users_quantity > 0) {
    // Reserve memory dynamically for the amount of results
    list.users = (User *)malloc(sizeof(User) * list.users_quantity);

    // Set the start index for getting the information from the frame
    index = countDigits(list.users_quantity) + 1;

    // Process all the data from the received frame
    do {
      // Get username
      list.users[users_processed_counter].username = readFromFrameUntilDelimiter(frame.data, index, '*');

      // Move the index
      index += strlen(list.users[users_processed_counter].username) + 1;

      // Get user ID
      buffer = readFromFrameUntilDelimiter(frame.data, index, '*');
      list.users[users_processed_counter].id = atoi(buffer);
      free(buffer);
      
      // Move the index
      index += countDigits(list.users[users_processed_counter].id) + 1;

      // Increase the number of processed users results
      users_processed_counter++;
    } while (frame.data[index] != '\0');

    // Check if all results have been received
    if (users_processed_counter < list.users_quantity) {
      // Keep receiving frames with the data remaining
      do {
        // Receive another response 
        frame = receiveFrame(socket_fd);

        // Reset index on read another frame
        index = 0;

        // Process all the data from the received frame
        do {
          // Get username
          list.users[users_processed_counter].username = readFromFrameUntilDelimiter(frame.data, index, '*');

          // Move the index
          index += strlen(list.users[users_processed_counter].username) + 1;

          // Get user ID
          buffer = readFromFrameUntilDelimiter(frame.data, index, '*');
          list.users[users_processed_counter].id = atoi(buffer);
          free(buffer);
          
          // Move the index
          index += countDigits(list.users[users_processed_counter].id) + 1;

          // Increase the number of processed users results
          users_processed_counter++;
        } while (frame.data[index] != '\0');
      } while (users_processed_counter < list.users_quantity);
    }

    // Print users list
    sprintf(text, "Hi ha %d persones humanes a %s\n", list.users_quantity, zip_code);
    printMessage(text);

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

void simulateBashShell(FremenConfiguration fremen_configuration) {
  char *buffer = NULL, **command = NULL, *frame = NULL, text[MAX_LENGTH];
  int args_counter = 0;
  Frame received_frame;

  printMessage("Benvingut a Fremen");

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
              sendFrame(ORIGIN_FREMEN, socket_fd, frame);
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
                
                sprintf(text, "Benvingut %s. Tens ID %d.\nAra estàs connectat a atreides\n", username, user_id);
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
            sendFrame(ORIGIN_FREMEN, socket_fd, frame);
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
          


        }
      } else if (strcmp(command[0], custom_commands[3]) == 0) {
        if (args_counter > PHOTO_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda PHOTO\n");
        } else if (args_counter < PHOTO_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda PHOTO\n");
        } else {
          printMessage("PHOTO\n");
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
            sendFrame(ORIGIN_FREMEN, socket_fd, frame);
            free(frame);

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

      // Free buffer and command
      free(buffer);
      free(command);
    }
  }
}