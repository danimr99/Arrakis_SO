#include "harkonen_module.h"

/*
 * Function that retrieves the process owner.
 * 
 * @return Name of the process owner.
*/
char *getProcessOwner() {
  int pipe_fd[2], index = 0;
  pid_t pid;
  char *process_owner = NULL, character;

  // Create a pipe and check result
  if (pipe(pipe_fd) == -1) {
    printMessage("ERROR: No s'ha pogut crear una pipe per a comunicar-se entre processos\n");
  }

  // Execute md5sum command on a child and get its PID
  if ((pid = fork()) == 0) {
    // Code to be executed by the child
    // Set STDOUT_FILENO as the file descriptor to use
    dup2(pipe_fd[1], STDOUT_FILENO);

    // Close unused write ends
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // Run whoami command
    execl("/bin/whoami", "whoami", NULL);
  } else {
    // Code to be executed by the parent
    // Wait until child has terminated its execution
    waitpid(pid, NULL, 0);

    // Close unused write end
    close(pipe_fd[1]);

    // Reserve memory for the first character of the process owner name
    process_owner = (char *)malloc(sizeof(char));

    // Get the process owner name
    do{   
      // Read process owner name character by character
      read(pipe_fd[0], &character, 1);

      // Reserve memory dynamically for the process owner name
      process_owner = (char *)realloc(process_owner, (index + 1) * sizeof(char));

      // Assign character to process owner name
      process_owner[index] = character;

      index++;
    } while (character != '\n');

    // Remove '\n' from the process owner name
    process_owner[index - 1] = '\0';

    // Close file descriptors
    close(pipe_fd[0]);

    return process_owner;
  }

  return NULL;
}

/*
 * Function that retrieves the PIDs from all the running Fremen processes 
 * executed by the process owner.
 * 
 * @param process_owner Name of the process owner.
 * @return All the PIDs splitted by space.
*/
char *getFremenProcessesByOwner(char *process_owner) {
  int pipe_fd[2], index = 0;
  pid_t pid;
  char *processes = NULL, character;

  // Create a pipe and check result
  if (pipe(pipe_fd) == -1) {
    printMessage("ERROR: No s'ha pogut crear una pipe per a comunicar-se entre processos\n");
  }

  // Execute md5sum command on a child and get its PID
  if ((pid = fork()) == 0) {
    // Code to be executed by the child
    // Set STDOUT_FILENO as the file descriptor to use
    dup2(pipe_fd[1], STDOUT_FILENO);

    // Close unused write ends
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // Execute bash script to get all fremen PIDs
    execlp("/bin/sh", "/bin/sh", BASH_SCRIPT, process_owner, NULL);

    // Free up memory
    free(process_owner);
  } else {
    // Code to be executed by the parent
    // Wait until child has terminated its execution
    waitpid(pid, NULL, 0);

    // Close unused write end
    close(pipe_fd[1]);

    // Reserve memory for the first character of the process owner name
    processes = (char *)malloc(sizeof(char));

    // Get the process owner name
    do {   
      // Read process owner name character by character
      read(pipe_fd[0], &character, 1);

      // Reserve memory dynamically for the process owner name
      processes = (char *)realloc(processes, (index + 1) * sizeof(char));

      // Assign character to process owner name
      processes[index] = character;

      index++;
    } while (character != '\n');

    // Remove '\n' from the process owner name
    processes[index - 1] = '\0';

    // Close file descriptors
    close(pipe_fd[0]);

    return processes;
  }

  return NULL;
}

/*
 * Function that generates a number between 0 and max.
 * 
 * @param max Maximum number.
 * @return Random number generated.
*/
int generateRandom(int max) {
  return rand() % max;
}

/*
 * Function that kills a Fremen process by PID.
 * 
 * @param process_pid PID of a Fremen process.
*/
void killProcess(char *process_pid) {
  char text[MAX_LENGTH];
  pid_t pid;

  // Execute Linux command on a child and get its PID
  if ((pid = fork()) == 0) {
    // Code to be executed by the child
    // Execute Linux command and get the result of the operation
    int status_code = execlp("/bin/kill", "/bin/kill", "-9", process_pid, NULL);

    // Check if execution of the Linux command has been an error
    if (status_code == -1) {
      printMessage("ERROR: Comanda desconeguda\n");
    }

    // Exit child process
    exit(0);
  } else {
    // Code to be executed by the parent
    // Wait until child has terminated its execution
    waitpid(pid, NULL, 0);

    sprintf(text, "S'ha enviat un SIGKILL al procés Fremen amb PID %s\n", process_pid);
    printMessage(text);
  }
}