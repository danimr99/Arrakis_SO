#include "harkonen_module.h"

// Global variables
int time;
char *process_owner;
FremenProcessesList fremen_list;

// Function to handle signals
void RsiHandler(int signal_number) {
  char *processes = NULL, text[MAX_LENGTH];
  int random;

  // Signal received is a SIGINT (Ctrl + C)
  if (signal_number == SIGINT) {
    printMessage("\nAturant Harkonen...\n");

    // Freee up memory
    free(process_owner);

    // Check if exists any Fremen process to free up memory
    if (fremen_list.fremen_quantity > 0) {
      // Free up memory for the Fremen list
      free(processes);
      free(fremen_list.fremen_pids);
    }

    // Reprogram Ctrl + C signal (SIGINT) to default behaviour
    signal(SIGINT, SIG_DFL);

    // Self shutdown program using Ctrl + C signal (SIGINT)
    raise(SIGINT);
  }

  // Signal received is a SIGALRM
  if (signal_number == SIGALRM) {
    printMessage("\nEscanejant en busca de processos Fremen...\n");

    // Get string of Fremen PIDs
    processes = getFremenProcessesByOwner(process_owner);

    // Convert string to list of PIDs and get quantity of processes found
    fremen_list.fremen_pids = split(processes, " ");
    fremen_list.fremen_quantity = countSplits(fremen_list.fremen_pids);

    // Print message depending on the quantity of processes found
    if (fremen_list.fremen_quantity == 0) {
      printMessage("No s'ha trobat cap procés Fremen\n");
    } else if (fremen_list.fremen_quantity == 1) {
      printMessage("S'ha trobat 1 procés Fremen\n");
    } else {
      sprintf(text, "S'han trobat %d processos Fremen\n", fremen_list.fremen_quantity);
      printMessage(text);
    }

    // Check if exists any Fremen process
    if (fremen_list.fremen_quantity > 0) {
      // Get random index for the list of Fremen processes
      random = generateRandom(fremen_list.fremen_quantity);

      // Kill a random Fremen
      killProcess(fremen_list.fremen_pids[random]);

      // Free up memory for the Fremen list
      free(processes);
      free(fremen_list.fremen_pids);

      // Reset Fremen counter from the list
      fremen_list.fremen_quantity = 0;
    }

    // Schedule time period recursively
    alarm(time);
  }
}

int main(int argc, char **argv) {
  printMessage("Iniciant Harkonen...\n");

  // Check if time period parameter has been specified
  if (argc < 2) {
    printMessage("ERROR: Falta especificar el temps d'espera\n");

    return 0;
  }

  // Get the time period passed as parameter
  time = atoi(argv[1]);

  // Get process owner
  process_owner = getProcessOwner();

  // Set default quantity of Fremen processes
  fremen_list.fremen_quantity = 0;
  
  // Handle Ctrl + C signal (SIGINT)
  signal(SIGINT, (void *)RsiHandler);

  // Handle SIGALRM signal
  signal(SIGALRM, (void *)RsiHandler);

  // Schedule time period for the first time
  alarm(time);

  while (TRUE) {
    pause();
  }
  
  // Self shutdown program using Ctrl + C signal (SIGINT)
  raise(SIGINT);

  return 0;
}