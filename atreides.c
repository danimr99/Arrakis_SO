#include "atreides_module.h"

// Global variables
AtreidesConfiguration atreides_configuration;
int socket_fd;
pthread_mutex_t mutex;
UsersList user_list;


// Function to handle signals
void RsiHandler(void) {
  printMessage("\nAturant Atreides...\n");

  // Free up memory
  free(atreides_configuration.ip);
  free(atreides_configuration.directory);

  // Close socket
  if (socket_fd > 0) {
    close(socket_fd);
  }

  // Destroy mutex
  pthread_mutex_destroy(&mutex);

  // Reprogram Ctrl + C signal (SIGINT) to default behaviour
  signal(SIGINT, SIG_DFL);

  // Self shutdown program using Ctrl + C signal (SIGINT)
  raise(SIGINT);
}

int main(int argc, char **argv) {
  int config_file_fd;

  printMessage("SERVIDOR ATREIDES\n");

  // Check if Atreides configuration file has been specified
  if (argc < 2) {
    printMessage("ERROR: Falta especificar el fitxer de configuració d'Atreides\n");

    return 0;
  }

  // Handle Ctrl + C signal (SIGINT)
  signal(SIGINT, (void *)RsiHandler);

  // Get file descriptor of Atreides configuration file
  config_file_fd = open(argv[1], O_RDONLY);

  // Check if Atreides configuration file exists
  if (config_file_fd > 0) {
    // Get data from Atreides configuration file
    atreides_configuration = getAtreidesConfiguration(config_file_fd);

    // Get users list
    user_list = getUsers();

    // Start server
    socket_fd = startServer(atreides_configuration.ip, atreides_configuration.port);

    // Handle connections to the Atreides server
    handleConnections(socket_fd, mutex);
  } else {
    printMessage("ERROR: No s'ha trobat el fitxer de configuració d'Atreides\n");
  }

  // Self shutdown program using Ctrl + C signal (SIGINT)
  raise(SIGINT);

  return 0;
}