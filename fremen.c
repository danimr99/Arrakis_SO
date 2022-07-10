#include "fremen_module.h"

// Global variables
FremenConfiguration fremen_configuration;
extern char *username;
extern int user_id, socket_fd;
extern DownloadedPhotosList downloaded_photos_list;

/*
 * Function that handles any signal that receives Fremen.
*/
void RsiHandler(int signal_number) {
  char *frame = NULL;

  // Signal received is a SIGINT (Ctrl + C)
  if (signal_number == SIGINT) {
    printMessage("\nAturant Fremen...\n");

    // Free up memory
    free(fremen_configuration.ip);
    free(fremen_configuration.directory);

    // Check if Fremen client has downloaded any photo from Atreides
    if (downloaded_photos_list.photos_quantity > 0) {
      // Iterate through the list of downloaded photos
      for (int i = 0; i < downloaded_photos_list.photos_quantity; i++) {
        // Free memory for the photo name deleted
        free(downloaded_photos_list.photo_names[i]);
      }

      free(downloaded_photos_list.photo_names);
    }

    // Close socket connection if exists
    if (socket_fd > 0) {
      // Generate logout frame
      frame = initializeFrame(ORIGIN_FREMEN);
      frame = generateRequestLogoutFrame(frame, LOGOUT_TYPE, username, user_id);

      // Send logout frame
      sendFrame(socket_fd, frame);
      free(frame);
      free(username);

      // Close socket connection
      close(socket_fd);
      socket_fd = 0;
    }

    // Reprogram Ctrl + C signal (SIGINT) to default behaviour
    signal(SIGINT, SIG_DFL);

    // Self shutdown program using Ctrl + C signal (SIGINT)
    raise(SIGINT);
  }

  // Signal received is a SIGALRM
  if (signal_number == SIGALRM) {
    // Schedule Fremen configuration clean time period recursively
    alarm(fremen_configuration.clean_time);

    // Check if Fremen client has downloaded any photo from Atreides
    if (downloaded_photos_list.photos_quantity > 0) {
      // Delete all files downloaded from Atreides
      deleteDownloadedPhotos();
    }
  }
}

int main(int argc, char **argv) {
  int config_file_fd;

  // Check if fremen configuration file has been specified
  if (argc < 2) {
    printMessage("ERROR: Falta especificar el fitxer de configuració de Fremen\n");

    return 0;
  } 

  // Handle Ctrl + C signal (SIGINT)
  signal(SIGINT, (void *)RsiHandler);

  // Get file descriptor of Fremen configuration file
  config_file_fd = open(argv[1], O_RDONLY);

  // Check if Fremen configuration file exists
  if (config_file_fd > 0) {
    // Get data from Fremen configuration file
    fremen_configuration = getFremenConfiguration(config_file_fd);

    // Handle SIGALRM signal
    signal(SIGALRM, (void *)RsiHandler);

    // Schedule Fremen configuration clean time period for the first time
    alarm(fremen_configuration.clean_time);

    // Simulate bash shell
    simulateBashShell(fremen_configuration);
  } else {
    printMessage("ERROR: No s'ha trobat el fitxer de configuració de Fremen\n");
  }

  // Self shutdown program using Ctrl + C signal (SIGINT)
  raise(SIGINT);

  return 0;
}