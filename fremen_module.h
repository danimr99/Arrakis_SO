#include "generic_module.h"
#include "frame_module.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Data structure for the Fremen file configuration
typedef struct {
  int clean_time;
  char *ip;
  int port;
  char *directory;
} FremenConfiguration;

typedef struct {
  int id;
  char *username;
  char *zip_code;
} User;

typedef struct {
  int users_quantity;
  User *users;
} UsersList;

#define LOGIN_REQUIRED_PARAMETERS 2
#define SEARCH_REQUIRED_PARAMETERS 1
#define SEND_REQUIRED_PARAMETERS 1
#define PHOTO_REQUIRED_PARAMETERS 1
#define LOGOUT_REQUIRED_PARAMETERS 0

FremenConfiguration getFremenConfiguration(int config_file_fd);

void simulateBashShell(FremenConfiguration fremen_configuration);

void runLinuxCommand(char **command);