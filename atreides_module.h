#include "generic_module.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

// Data structure for the Atreides file configuration
typedef struct {
  char *ip;
  int port;
  char *directory;
} AtreidesConfiguration;

#define MAX_CLIENT_REQUESTS 10

AtreidesConfiguration getAtreidesConfiguration(int config_file_fd);

int startServer(char *ip, int port);

void handleConnections(int socket_fd);