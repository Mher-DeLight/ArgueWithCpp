#include "../include/argwc.h"
#include "../include/Parser.h"
#include "../include/Tokenizer.h"
#include <iostream>
#include <unordered_set>

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
void argwc::print_arguments() {
    std::cout << "== PROGRAM ARGUMENTS ==" << std::endl;
    for (int i = 0; i < argc; i++) {
        std::cout << std::to_string(i) << ") " << argv[i];
        if (i == 0) {
            std::cout << " (by default)";
        }
        std::cout << std::endl;
    }
}

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
    std::unordered_set<std::string> provided_args;
    for (int i = 1; i < argc; ++i) { // 1 is the program name, skip it
        provided_args.insert(std::string(argv[i]));
    }

    // we'll build active object list starting from top-level Objects, when a flag
    // with an if_passed block is present in rhe args, its block children become active too
    std::vector<Object*> active_objs;
    active_objs.reserve(objects.size());
    for (auto& obj : objects) {
        active_objs.push_back(obj.get());
    }

    std::unordered_set<std::string> activated_flags; // flag names whose blocksw ere expanded
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto* obj : active_objs) {
            if (auto flg = dynamic_cast<Object_Flag*>(obj)) {
                if (flg->block && provided_args.contains(flg->flag_text) &&
                    !activated_flags.contains(flg->name)) {
                    for (auto& child : flg->block->children) {
                        active_objs.push_back(child.get());
                    }
                    activated_flags.insert(flg->name);
                    changed = true;
                }
            }
        }
    }

    // now we determine flag values and enforce requirements (only for active flags)
    for (auto* obj : active_objs) {
        if (auto flg = dynamic_cast<Object_Flag*>(obj)) {
            if (provided_args.contains(flg->flag_text)) {
                vars_flags[flg->name] = true;
            } else {
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
}
bool argwc::flag_enabled(const std::string& varname) {
    if (!vars_flags.contains(varname)) {
        panic("Cannot get the value of flag \"" + varname + "\" as it does not exist");
    }
    return vars_flags[varname];
}