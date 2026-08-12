#include "../include/argwc.h"

argwc::argwc() {
    std::fstream grammer_file(config_file_path);
    if (!grammer_file) {
        panic("Could not find configuration file \"" + config_file_path + "\"");
    }

    std::stringstream buffer;
    buffer << grammer_file.rdbuf();
    config_file = buffer.str();
    parse_config_file();
};

void argwc::parse_config_file() {}