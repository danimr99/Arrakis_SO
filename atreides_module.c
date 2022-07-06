#include "atreides_module.h"

extern UsersList user_list;
extern AtreidesConfiguration atreides_configuration;

AtreidesConfiguration getAtreidesConfiguration(int config_file_fd) {
  AtreidesConfiguration atreides_configuration;
  char *buffer = NULL;

  // Get IP from Atreides configuration file
  atreides_configuration.ip = readLineUntilDelimiter(config_file_fd, '\n');

  // Get directory from Atreides configuration file
  buffer = readLineUntilDelimiter(config_file_fd, '\n');
  atreides_configuration.port = atoi(buffer);
  free(buffer);

  // Get directory from Atreides configuration file
  buffer = readLineUntilDelimiter(config_file_fd, '\n');
  atreides_configuration.directory = (char *)malloc(sizeof(char) * strlen(buffer));

  // Iterate through the string skipping the first character ('/')
  for (int i = 1; buffer[i] != '\0'; i++) {
    atreides_configuration.directory[i - 1] = buffer[i];
  }

  free(buffer);

  // Create configuration directory if it does not exist
  struct stat stats = {0};

  if (stat(atreides_configuration.directory, &stats) == -1) {
    mkdir(atreides_configuration.directory, 0700);
  }

  printMessage("Llegit el fitxer de configuració\n");

  // Close configuration file descriptor
  close(config_file_fd);

  return atreides_configuration;
}

UsersList getUsers() {
  char *buffer = NULL;
  UsersList list;
  int users_list_fd, i = 0;

  // Open file containing all the registered users
  users_list_fd = open(USERS_REGISTER_PATH, O_RDONLY);

  // Check if file does not exist
  if (users_list_fd < 1) {
    // Create file
    users_list_fd = open(USERS_REGISTER_PATH, O_CREAT | O_RDWR, 0666);

    // Check if file has been successfully created
    if (users_list_fd > 0) {
      // Write default quantity of users to the newly created register file
      write(users_list_fd, "0\n", 2);
      close(users_list_fd);
    } else {
      printMessage("ERROR: No s'ha pogut trobar ni crear el registre d'usuaris\n");
      raise(SIGINT);
    }
  }

  // Get the quantity of users from the register file
  buffer = readLineUntilDelimiter(users_list_fd, '\n');
  list.users_quantity = atoi(buffer);
  free(buffer);

  // Reserve memory dynamically for the quantity of existing users on the file
  list.users = (User *)malloc(sizeof(User) * list.users_quantity);

  // Get every user from the file
  while(i < list.users_quantity) {
    // Get the user ID
    buffer = readLineUntilDelimiter(users_list_fd, '-');
    list.users[i].id = atoi(buffer);
    free(buffer);

    // Get the user name
    list.users[i].username = readLineUntilDelimiter(users_list_fd, '-');

    // Get the user zip code
    list.users[i].zip_code = readLineUntilDelimiter(users_list_fd, '\n');

    i++;
  }

  // Close file
  close(users_list_fd);

  return list;
}

void addUser(User user) {
  // Reserve memory dynamically for the list of users plus the one to be added
  user_list.users = (User *)realloc(user_list.users, sizeof(User) * user_list.users_quantity);

  // Reserve memory dynamically for the username and the zip code of the user to be added
  user_list.users[user.id - 1].username = (char *)malloc((strlen(user.username) + 1) * sizeof(char));
  user_list.users[user.id - 1].zip_code = (char *)malloc((strlen(user.zip_code) + 1) * sizeof(char));

  // Assign values to the list
  user_list.users[user.id - 1].id = user.id;
  strcpy(user_list.users[user.id - 1].username, user.username);
  strcpy(user_list.users[user.id - 1].zip_code, user.zip_code);
  user_list.users[user.id - 1].client_fd = user.client_fd;
  user_list.users[user.id - 1].process = user.process;
} 

void updateUsers() {
  char buffer[MAX_LENGTH];
  int users_list_fd;

  // Open file containing all the registered users
  users_list_fd = open(USERS_REGISTER_PATH, O_WRONLY);

  // Check if file exists
  if (users_list_fd > 0) {
    // Write the new quantity of users registered
    sprintf(buffer, "%d\n", user_list.users_quantity);
    write(users_list_fd, buffer, sizeof(char) * strlen(buffer));

    // Iterate through each user from the list
    for (int i = 0; i < user_list.users_quantity; i++) {
      // Write each user formatted
      sprintf(buffer, "%d-%s-%s\n", user_list.users[i].id, user_list.users[i].username, user_list.users[i].zip_code);
      write(users_list_fd, buffer, sizeof(char) * strlen(buffer));
    }

    // Close file
    close(users_list_fd);
  } else {
    printMessage("ERROR: No s'ha pogut trobar per modificar el registre d'usuaris\n");
    raise(SIGINT);
  }
}

int generateUserID() {
  user_list.users_quantity += 1;

  return user_list.users_quantity;
}

int existsUser(User user) {
  // Iterate through each user on the list
  for (int i = 0; i < user_list.users_quantity; i++) {
    // Check if user exists on the register (username and zip code matches)
    if (strcmp(user_list.users[i].username, user.username) == 0 && strcmp(user_list.users[i].username, user.username) == 0) {
      return TRUE;
    }
  } 

  return FALSE;
}

int getUserID(char *username, char *zip_code) {
  int id;

  // Iterate through each user on the list 
  for (int i = 0; i < user_list.users_quantity; i++) {
    // Check if is the user
    if (strcmp(user_list.users[i].username, username) == 0 && strcmp(user_list.users[i].zip_code, zip_code) == 0) {
      id = user_list.users[i].id;
      break;
    }
  } 

  return id;
}

User getUserByID(int id) {
  User user;

  // Iterate through the list of users
  for (int i = 0; i < user_list.users_quantity; i++) {
    // Check if user matches with the ID introduced
    if (user_list.users[i].id == id) {
      user = user_list.users[i];
      break;
    }
  }

  return user;
}

int startServer(char *ip, int port) {
  char buffer[MAX_LENGTH];
  int listen_fd;
  struct sockaddr_in server;

  // Check if port specified on Atreides configuration file can start listening connections requests
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    sprintf(buffer, "ERROR: No s'ha pogut crear el socket a %s:%d.\n", ip, port);
    printMessage(buffer);

    return 0;
  }

  // Set up server configuration
  memset(&server, 0, sizeof(server));
  server.sin_port = htons(port);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  // Check if port specified on Atreides configuration file is binding
  if (bind(listen_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    sprintf(buffer, "ERROR: No s'ha pogut bindear a %s:%d.\n", ip, port);
    printMessage(buffer);

    return 0;
  }

  // Check if port specified on Atreides configuration file is listening
  if (listen(listen_fd, MAX_CLIENT_REQUESTS) < 0) {
    sprintf(buffer, "ERROR: No s'ha pogut fer listen a %s:%d.\n", ip, port);
    printMessage(buffer);

    return 0;
  }

  return listen_fd;
}

User getUserFromFrame(char frame_data[FRAME_DATA_LENGTH]) {
  User user;

  // Read username from frame
  user.username = readFromFrameUntilDelimiter(frame_data, 0, '*');

  // Read zip code from frame
  user.zip_code = readFromFrameUntilDelimiter(frame_data, strlen(user.username) + 1, '\0');

  // Set default user ID
  user.id = 0;

  return user;
}

void getUsersDataByZipCode(int client_fd, pthread_mutex_t *mutex, char *zip_code) {
  char *data = NULL, text[MAX_LENGTH], *send_frame = NULL;
  int users_counter = 0;

  // Get number of users from the zip code introduced
  for (int i = 0; i < user_list.users_quantity; i++) {
    // Check if user is from the specified zip code
    if (strcmp(user_list.users[i].zip_code, zip_code) == 0) {
      users_counter++;
    }
  }

  // Add number of users to data
  asprintf(&data, "%d", users_counter);

  // Allow only one user at the time to check and modify information
  pthread_mutex_lock(mutex);

  // Print the number of users matching the zip code specified
  sprintf(text, "Feta la cerca\nHi ha %d persones humanes a %s\n", users_counter, zip_code);
  printMessage(text);

  // Iterate through each user on the list
  for (int i = 0; i < user_list.users_quantity; i++) {
    // Check if user is from the specified zip code
    if (strcmp(user_list.users[i].zip_code, zip_code) == 0) {
      // Print user
      sprintf(text, "%d %s\n", user_list.users[i].id, user_list.users[i].username);
      printMessage(text);

      // Check if user can be added to the data
      if ((strlen(data) + strlen(user_list.users[i].username) + countDigits(user_list.users[i].id)) > FRAME_DATA_LENGTH) {
        // Send existing data
        send_frame = initializeFrame(ORIGIN_ATREIDES);
        send_frame = generateResponseSearchFrame(send_frame, SEARCH_SUCCESSFUL_TYPE, data);
        sendFrame(ORIGIN_ATREIDES, client_fd, send_frame);
        free(data);
        free(send_frame);

        // Add user to data
        asprintf(&data, "%s*%d", user_list.users[i].username, user_list.users[i].id);
      } else {
        // Add user to data
        asprintf(&data, "%s*%s*%d", data, user_list.users[i].username, user_list.users[i].id);
      }      
    }
  }

  // Unblock restriction
  pthread_mutex_unlock(mutex);

  // Check if there is data remaining to be sent
  if (strlen(data) > 0) {
    send_frame = initializeFrame(ORIGIN_ATREIDES);
    send_frame = generateResponseSearchFrame(send_frame, SEARCH_SUCCESSFUL_TYPE, data);
    sendFrame(ORIGIN_ATREIDES, client_fd, send_frame);
    free(data);
    free(send_frame);
  }
}

void *runClientThread(void *args) {
  char buffer[MAX_LENGTH], *send_frame, *zip_code;
  Frame frame;
  User user;
  Photo photo;
  int is_exit = FALSE;

  while (!is_exit) {
    // Receive frame
    frame = receiveFrame(((ClientThreadArgs *)args)->client_fd);

    // Check the frame type of the frame received
    switch (frame.type) {
      case LOGIN_TYPE:
        // Get user from frame
        user = getUserFromFrame(frame.data);

        // Allow only one user at the time to check and modify information
        pthread_mutex_lock(((ClientThreadArgs *)args)->mutex);

        // Check if user exists
        if (existsUser(user)) {
          // Get user ID
          user.id = getUserID(user.username, user.zip_code);
        } else {
          // Generate an ID for the user
          user.id = generateUserID();
        }

        // Assign to user its client socket and process
        user.client_fd = ((ClientThreadArgs *) args)->client_fd;
        user.process = pthread_self();

        // Add user to the list and update the users file
        addUser(user);
        updateUsers();

        sprintf(buffer, "Rebut login de %s %s\nAssignat a ID %d\n", user.username, user.zip_code, user.id);
        printMessage(buffer);

        // Unblock restriction
        pthread_mutex_unlock(((ClientThreadArgs *)args)->mutex);

        // Send response to the Fremen client
        send_frame = initializeFrame(ORIGIN_ATREIDES);
        send_frame = generateResponseLoginFrame(send_frame, LOGIN_SUCCESSFUL_TYPE, user.id);
        sendFrame(ORIGIN_ATREIDES, user.client_fd, send_frame);

        // Free up memory of the frame sent
        free(send_frame);

        break;
      
      case SEARCH_TYPE:
        // Receive user data from the request frame
        user.username = readFromFrameUntilDelimiter(frame.data, 0, '*');
        user.id = atoi(readFromFrameUntilDelimiter(frame.data, strlen(user.username) + 1, '*'));
        zip_code = readFromFrameUntilDelimiter(frame.data, strlen(user.username) + 2 + countDigits(user.id), '\0');

        sprintf(buffer, "Rebut search %s de %s %d\n", zip_code, user.username, user.id);
        printMessage(buffer);

        // Send users matching with the zip code as a response to the Fremen client
        getUsersDataByZipCode(((ClientThreadArgs *)args)->client_fd, ((ClientThreadArgs *)args)->mutex, zip_code);

        break;

      case PHOTO_INFO_TYPE:
        // Receive photo information frame
        photo = receivePhotoInformationFrame(frame.data);

        sprintf(buffer, "Rebut send %s de %s %d\n", photo.name, user.username, user.id);
        printMessage(buffer);

        // FIXME: Receive photo transfer from Fremen using frames and response with the result
        if (strcmp(photo.hash, "\0") != 0) {
          processPhotoFrame(user.id, ((ClientThreadArgs *)args)->client_fd, atreides_configuration.directory, photo);
        } else {
          sprintf(buffer, "ERROR: L'usuari %s amb ID %d ha intentat enviar una foto que no existeix\n", user.username, user.id);
          printMessage(buffer);
        }

        break;

      // TODO : All remaining cases (P)

      case LOGOUT_TYPE:
        // Stop client thread loop
        is_exit = TRUE;

        // Receive user data from the request frame
        user.username = readFromFrameUntilDelimiter(frame.data, 0, '*');
        user.id = atoi(readFromFrameUntilDelimiter(frame.data, strlen(user.username) + 1, '\0'));
        
        sprintf(buffer, "Rebut logout de %s %d\nDesconnectat d'Atreides\n", user.username, user.id);
        printMessage(buffer);

        // FIXME: Free also on Atreides RsiHandler 
        // Free up memory of the user and the frame sent
        free(user.username);
        free(user.zip_code);

        // Detach thread and cancel it 
        pthread_detach(user.process);
        pthread_cancel(user.process);

        break;

      default:
        // TODO: Implementation of "Erroneous frames detection" from the PDF (EOF)
        printMessage("ERROR: S'ha rebut un frame de tipus desconegut\n");

        break;
    }
  }

  // Disconnect client from the Atreides server
  close(((ClientThreadArgs *)args)->client_fd);

  return NULL;
}

void handleConnections(int socket_fd, pthread_mutex_t mutex) {
  int client_fd;
  pthread_t thread;
  ClientThreadArgs args;

  // Initialize mutex 
  if (pthread_mutex_init(&mutex, NULL) != 0) {
    printMessage("ERROR: No s'ha pogut inicialitzar el mutex\n");
    close(socket_fd);
    raise(SIGINT);
  }

  while (TRUE) {
    printMessage("Esperant connexions...\n");

    // Accept connections
    client_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL);

    // Set up arguments for client thread
    args.client_fd = client_fd;
    args.mutex = &mutex;

    // Create a thread for each client connection
    pthread_create(&thread, NULL, runClientThread, &args);
  }
}