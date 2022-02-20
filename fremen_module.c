#include "fremen_module.h"

FremenConfiguration getFremenConfiguration(int config_file_fd) {
    FremenConfiguration fremen_configuration;

    fremen_configuration.clean_time = atoi(readLineUntilDelimiter(config_file_fd, '\n'));
    fremen_configuration.ip = readLineUntilDelimiter(config_file_fd, '\n');
    fremen_configuration.port = atoi(readLineUntilDelimiter(config_file_fd, '\n'));
    fremen_configuration.directory = readLineUntilDelimiter(config_file_fd, '\n');

    return fremen_configuration;
}