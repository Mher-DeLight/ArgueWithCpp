#pragma once
#include "Common.h"
#include "ErrorHandler.h"
#include <fstream>
#include <map>
#include <span>
#include <sstream>
#include <vector>

class argwc {
private:
    std::map<std::string, std::string> vars_args;
    std::map<std::string, bool> vars_flags;

    std::vector<unsigned char> file_data;
    std::vector<std::unique_ptr<Object>> objects;

    void read_config();
    void read_arguments();
    int argc = 0;
    char** argv;

public:
    argwc(int argc_, char** argv_, const std::span<uint8_t> data);

    void print_arguments();
    bool flag_enabled(const std::string& varname);
    std::string get_arg(const std::string& varname);

    bool flag_exists(const std::string& flagname);
    bool arg_exists(const std::string& argname);
    bool obj_exists(const std::string& objname);
};