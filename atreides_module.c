#include "atreides_module.h"

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
  atreides_configuration.directory = readLineUntilDelimiter(config_file_fd, '\n');

  printMessage("Llegit el fitxer de configuració\n");

  // Close configuration file descriptor
  close(config_file_fd);

  return atreides_configuration;
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

void *runClientThread(void *args) {
  /*pthread_mutex_lock(((ClientThreadArgs *)args)->mutex);
  printMessage("This is the client thread\n");
  pthread_mutex_unlock(((ClientThreadArgs *)args)->mutex);*/

  //char *send_frame;
  Frame frame;
  int is_exit = FALSE;

  while (!is_exit) {
    // Receive frame
    frame = receiveFrame(((ClientThreadArgs *)args)->client_fd);

    // Check the frame type of the frame received
    switch (frame.type) {
    case LOGIN_TYPE:
      // TODO : Login logic
      break;
    
    // TODO : All remaining cases (S, F, P)

    case LOGOUT_TYPE:
      // TODO : Logout logic
      // Stop client thread loop
      is_exit = TRUE;

      break;

    default:
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