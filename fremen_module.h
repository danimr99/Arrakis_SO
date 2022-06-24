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

#define LOGIN_REQUIRED_PARAMETERS 2
#define SEARCH_REQUIRED_PARAMETERS 1
#define SEND_REQUIRED_PARAMETERS 1
#define PHOTO_REQUIRED_PARAMETERS 1
#define LOGOUT_REQUIRED_PARAMETERS 0

FremenConfiguration getFremenConfiguration(int config_file_fd);

int simulateBashShell(FremenConfiguration fremen_configuration);

void runLinuxCommand(char **command);