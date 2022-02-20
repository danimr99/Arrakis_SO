#include "fremen_module.h"

// Function to handle signals
void RsiHandler(void) {
    printMessage("\nAturant Fremen...\n");
    
    // Reprogram Ctrl + C signal (SIGINT) to default behaviour
    signal(SIGINT, SIG_DFL);

    // Self shutdown program using Ctrl + C signal (SIGINT)
    raise(SIGINT);
}


int main(int argc, char ** argv) {
    char buffer[MAX_LENGTH];
    FremenConfiguration fremen_configuration;

    // Check if fremen configuration file has been specified
    if(argc < 2) {
        printMessage("ERROR: Falta especificar el fitxer de configuració de Fremen.\n");
        
        return 0;
    }

    // Print the name of the configuration file specified
    sprintf(buffer, "Configuration file: %s\n", argv[1]);
    printMessage(buffer);

    // Handle Ctrl + C signal (SIGINT)
    signal(SIGINT, (void *) RsiHandler);
    
    // TODO: Logic of the program goes here

    // Self shutdown program using Ctrl + C signal (SIGINT)
    raise(SIGINT);

    return 0;
}