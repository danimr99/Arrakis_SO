#include "generic_module.h"

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

void simulateBashShell();

void runLinuxCommand(char ** command);