#pragma once
#include "Common.h"
#include "ErrorHandler.h"
#include <fstream>
#include <map>
#include <sstream>

class argwc {
private:
    std::map<std::string, std::string> vars_args;
    std::map<std::string, bool> vars_flags;

    std::string config_file_path = "ArgGrammar.awc";
    std::string config_file = "";
    std::vector<std::unique_ptr<Object>> objects;

    void read_config();
    void read_arguments();
    int argc = 0;
    char** argv;

public:
    argwc(int argc_, char** argv_);

    void print_arguments();
    bool get_flag_value(const std::string& varname);
};