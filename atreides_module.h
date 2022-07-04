#define _GNU_SOURCE
#include "generic_module.h"
#include "frame_module.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/stat.h>

// Data structure for the Atreides file configuration
typedef struct {
  char *ip;
  int port;
  char *directory;
} AtreidesConfiguration;

typedef struct {
  int client_fd;
  pthread_mutex_t *mutex;
} ClientThreadArgs;

typedef struct {
  int id;
  char *username;
  char *zip_code;
  int client_fd;
  pthread_t process;
} User;

typedef struct {
  int users_quantity;
  User *users;
} UsersList;

#define USERS_REGISTER_PATH "users_list.txt"
#define MAX_CLIENT_REQUESTS 10

AtreidesConfiguration getAtreidesConfiguration(int config_file_fd);

UsersList getUsers();

int startServer(char *ip, int port);

void handleConnections(int socket_fd, pthread_mutex_t mutex);