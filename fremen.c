#include "fremen_module.h"

// Global variables
FremenConfiguration fremen_configuration;

// Function to handle signals
void RsiHandler(void) {
    printMessage("\nAturant Fremen...\n");

    // Free up memory
    free(fremen_configuration.ip);
    free(fremen_configuration.directory);
    
    // Reprogram Ctrl + C signal (SIGINT) to default behaviour
    signal(SIGINT, SIG_DFL);

    // Self shutdown program using Ctrl + C signal (SIGINT)
    raise(SIGINT);
}

int main(int argc, char ** argv) {
    int config_file_fd;

    // Check if fremen configuration file has been specified
    if(argc < 2) {
        printMessage("ERROR: Falta especificar el fitxer de configuració de Fremen.\n");
        
        return 0;
    }

    // Handle Ctrl + C signal (SIGINT)
    signal(SIGINT, (void *) RsiHandler);
    
    /* Get file descriptor of Fremen configuration file */
    config_file_fd = open(argv[1], O_RDWR | O_APPEND | O_CREAT, 0666);

    // Check if Fremen configuration file exists
    if(config_file_fd > 0) {
        // Get data from Fremen configuration file
        fremen_configuration = getFremenConfiguration(config_file_fd);

        // Simulate bash shell
        simulateBashShell();
    } else {
        printMessage("ERROR: No s'ha trobat el fitxer de configuració de Fremen.\n");
    }

    // Self shutdown program using Ctrl + C signal (SIGINT)
    raise(SIGINT);

    return 0;
}