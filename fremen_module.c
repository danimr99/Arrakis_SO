#include "fremen_module.h"

// List of custom commands
const char *custom_commands[] = {"login", "search", "send", "photo", "logout"};

FremenConfiguration getFremenConfiguration(int config_file_fd) {
  FremenConfiguration fremen_configuration;
  char * buffer = NULL;

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
  fremen_configuration.directory = readLineUntilDelimiter(config_file_fd, '\n');

  return fremen_configuration;
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

void simulateBashShell() {
  char * buffer = NULL, * temp = NULL, ** command = NULL;
  int args_counter = 0, stop = FALSE;

  printMessage("Benvingut a Fremen");

  do {
    printMessage("\n$ ");

    // Get command introduced by the user
    buffer = readLineUntilDelimiter(STDIN_FILENO, '\n');

    // Format command to lower case
    temp = toLowerCase(buffer);
    free(buffer);

    // Check if command is not an empty string
    if(!isEmpty(temp)) {
      // Split command by spaces and get the number of arguments
      command = split(temp, " ");
      args_counter = countSplits(command) - 1;

      // Check if command matches with a custom or Linux command
      if (strcmp(command[0], custom_commands[0]) == 0) {
        if (args_counter > LOGIN_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda LOGIN\n");
        } else if (args_counter < LOGIN_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda LOGIN\n");
        } else {
          printMessage("LOGIN\n");
        }
      } else if (strcmp(command[0], custom_commands[1]) == 0) {
        if (args_counter > SEARCH_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda SEARCH\n");
        } else if (args_counter < SEARCH_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda SEARCH\n");
        } else {
          printMessage("SEARCH\n");
        }
      } else if (strcmp(command[0], custom_commands[2]) == 0) {
        if (args_counter > SEND_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Sobren paràmetres per a la comanda SEND\n");
        } else if (args_counter < SEND_REQUIRED_PARAMETERS) {
          printMessage("ERROR: Falten paràmetres per a la comanda SEND\n");
        } else {
          printMessage("SEND\n");
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
          stop = TRUE;
        }
      } else {
        // Execute Linux command
        runLinuxCommand(command);
      }

      // Free command array
      free(command);
    }

    // Free buffer
    free(temp);
  } while (!stop);
}