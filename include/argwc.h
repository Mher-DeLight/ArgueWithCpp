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

    void read_config();
    int argc;
    char** argv;

public:
    argwc(int argc_, char** argv_);
};