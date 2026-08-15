#include "../include/ArgueWithCpp/argwc.h"
#include "../include/ArgueWithCpp/Parser.h"
#include "../include/ArgueWithCpp/Tokenizer.h"
#include <iostream>
#include <unordered_set>

argwc::argwc(int argc_, char** argv_, const std::string& code)
    : argc(argc_), argv(argv_), config_file(code) {
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

    Parser parser;
    parser.load_tokens(tkn.get_tokens());
    parser.parse();

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

    std::unordered_set<std::string> activated_flags;
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
    // collect all known flag texts so we can separate positional args
    std::unordered_set<std::string> all_flag_texts;
    std::unordered_set<std::string> all_val_prefixes;
    for (auto& obj : objects) {
        if (auto flg = dynamic_cast<Object_Flag*>(obj.get())) {
            all_flag_texts.insert(flg->flag_text);
        }
        if (auto val = dynamic_cast<Object_Val*>(obj.get())) {
            all_val_prefixes.insert(val->prefix_text);
        }
    }

    // build positional args list (preserve order, skip known flag texts)
    std::vector<std::string> positional_args;
    for (int i = 1; i < argc; ++i) {
        std::string s(argv[i]);
        bool is_known = false;
        if (all_flag_texts.contains(s))
            is_known = true;
        else {
            for (auto& prefix : all_val_prefixes) {
                std::string pfx = prefix + "=";
                if (s.rfind(pfx, 0) == 0) {
                    is_known = true;
                    break;
                }
            }
        }

        if (!is_known)
            positional_args.push_back(s);
    }

    // assign positional args to active Object_Arg objects in encounter order
    size_t pos_idx = 0;
    // track which args' blocks we've expanded to avoid duplication
    std::unordered_set<std::string> activated_args;
    std::unordered_set<std::string> activated_vals;

    for (size_t i = 0; i < active_objs.size(); ++i) {
        auto* obj = active_objs[i];

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

        if (auto arg = dynamic_cast<Object_Arg*>(obj)) {
            if (pos_idx < positional_args.size()) {
                vars_args[arg->name] = positional_args[pos_idx++];

                if (arg->block && !activated_args.contains(arg->name)) {
                    for (auto& child : arg->block->children) {
                        active_objs.push_back(child.get());
                    }
                    activated_args.insert(arg->name);
                }
            }
        }

        if (auto val = dynamic_cast<Object_Val*>(obj)) {
            std::string match_prefix = val->prefix_text + "=";
            for (const auto& provided : provided_args) {
                if (provided.rfind(match_prefix, 0) == 0) {
                    vars_args[val->name] = provided.substr(match_prefix.size());

                    if (val->block && !activated_vals.contains(val->name)) {
                        for (auto& child : val->block->children) {
                            active_objs.push_back(child.get());
                        }
                        activated_vals.insert(val->name);
                    }
                    break;
                }
            }
        }
    }

    for (auto* obj : active_objs) {
        if (auto arg = dynamic_cast<Object_Arg*>(obj)) {
            if (!vars_args.contains(arg->name) && arg->required) {
                panic("Required argument \"" + arg->name + "\" was not passed");
                continue;
            } else if (!vars_args.contains(arg->name) && !arg->required) {
                vars_args[arg->name] = "";
                continue;
            }
        }
        if (auto val = dynamic_cast<Object_Val*>(obj)) {
            if (!vars_args.contains(val->name) && val->required) {
                panic("Required value \"" + val->prefix_text + "\" was not passed");
                continue;
            } else if (!vars_args.contains(val->name) && !val->required) {
                vars_args[val->name] = "";
                continue;
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
std::string argwc::get_arg(const std::string& varname) {
    if (!vars_args.contains(varname)) {
        panic("Cannot get the value of argument \"" + varname + "\" as it does not exist");
    }
    return vars_args[varname];
}

bool argwc::flag_exists(const std::string& flagname) {
    return vars_flags.contains(flagname);
}
bool argwc::arg_exists(const std::string& argname) {
    return vars_args.contains(argname);
}
bool argwc::obj_exists(const std::string& objname) {
    return vars_flags.contains(objname) || vars_args.contains(objname);
}