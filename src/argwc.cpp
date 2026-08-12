#include "../include/argwc.h"
#include "../include/Parser.h"
#include "../include/Tokenizer.h"
#include <iostream>

argwc::argwc(int argc_, char** argv_) {
    std::fstream grammer_file(config_file_path);
    if (!grammer_file) {
        panic("Could not find configuration file \"" + config_file_path + "\"");
    }

    std::stringstream buffer;
    buffer << grammer_file.rdbuf();
    config_file = buffer.str();
    read_config();
};

void argwc::read_config() {
    Tokenizer tkn;
    tkn.tokenize(config_file);
    tkn.pretty_print(std::cout);

    Parser parser;
    parser.load_tokens(tkn.get_tokens());
    parser.parse();
    parser.print_tree();
}