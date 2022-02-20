#include "generic_module.h"

// Data structure for the Fremen file configuration
typedef struct {
    int clean_time;
    char *ip;
    int port;
    char *directory;
} FremenConfiguration;

FremenConfiguration getFremenConfiguration(int config_file_fd);