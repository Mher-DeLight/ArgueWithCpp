#pragma once
#include "Common.h"
#include "ErrorHandler.h"
#include <fstream>
#include <sstream>

class argwc {
private:
    std::string config_file_path = "ArgGrammar.awc";
    std::string config_file = "";
    std::vector<std::unique_ptr<Object>> objects;

    void parse_config_file();

public:
    argwc();
};