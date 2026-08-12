#include "../include/argwc.h"
#include "../include/Parser.h"
#include "../include/Tokenizer.h"
#include <iostream>

argwc::argwc(int argc_, char** argv_) : argc(argc_), argv(argv_) {
    std::fstream grammer_file(config_file_path);
    if (!grammer_file) {
        panic("Could not find configuration file \"" + config_file_path + "\"");
    }

    std::stringstream buffer;
    buffer << grammer_file.rdbuf();
    config_file = buffer.str();
    read_config();
    read_arguments();
};

void argwc::read_config() {
    Tokenizer tkn;
    tkn.tokenize(config_file);
    tkn.pretty_print(std::cout);

    Parser parser;
    parser.load_tokens(tkn.get_tokens());
    parser.parse();
    parser.print_tree();

    objects = std::move(parser.entry_point->children);
}
void argwc::read_arguments() {
    for (int i = 0; i < argc; i++) {
        char* arg = argv[i];

        for (auto& obj : objects) {
            if (auto flg = dynamic_cast<Object_Flag*>(obj.get())) {
                if (flg->flag_text == arg) {
                    vars_flags[flg->name] = true;
                }
            }
        }
    }

    for (auto& obj : objects) {
        if (auto flg = dynamic_cast<Object_Flag*>(obj.get())) {
            if (!vars_flags.contains(flg->name) && flg->required) {
                panic("Required flag \"" + flg->flag_text + "\" was not passed");
                continue;
            } else if (!vars_flags.contains(flg->name) && !flg->required) {
                vars_flags[flg->name] = false;
                continue;
            }
        }
    }
}
bool argwc::get_flag_value(const std::string& varname) {
    if (!vars_flags.contains(varname)) {
        panic("Cannot get the value of flag \"" + varname + "\" as it does not exist");
    }
    return vars_flags[varname];
}