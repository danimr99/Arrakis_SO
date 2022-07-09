#define _GNU_SOURCE
#include "generic_module.h"

typedef struct {
  int fremen_quantity;
  char **fremen_pids;
} FremenProcessesList;

#define BASH_SCRIPT "./get_fremen_processes.sh"

char *getProcessOwner();

char *getFremenProcessesByOwner(char *process_owner);

int generateRandom(int max);

void killProcess(char *process_pid);