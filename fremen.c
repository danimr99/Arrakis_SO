#include "generic_module.h"
#include "fremen_module.h"

int main(int argc, char ** argv) {
    char buffer[MAX_LENGTH];

    // Check if fremen configuration file has been specified
    if(argc < 2) {
        printMessage("ERROR: Falta especificar el fitxer de configuració de Fremen.\n");
        
        return 0;
    }

    sprintf(buffer, "Configuration file: %s\n", argv[1]);
    printMessage(buffer);

    return 0;
}