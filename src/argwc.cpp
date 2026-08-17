#include "../include/ArgueWithCpp/argwc.h"
#include <iostream>
#include <unordered_set>

argwc::argwc(int argc_, char** argv_, const std::span<uint8_t> data)
    : argc(argc_), argv(argv_), file_data(std::vector<uint8_t>(data.begin(), data.end())) {
    entry_point = std::make_unique<Object_Block>();
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

std::unique_ptr<Object> argwc::readObject() {
    /*
    ==================================== FORMAT ===================================
    | Object type (0->invalid, 1->block, 2->arg, 3->flag, 4->val)         1 byte  |
    | Info (hgfedcba, a->is required, b->is ordered)                      1 byte  |
    |                                                                             |
    | Name size                                                           1 byte  |
    | Name                                                      (name size) bytes |
    |                                                                             |
    | Varname size                                                        1 byte  |
    | Varname                                                (varname size) bytes |
    |                                                                             |
    | Child count (sucessors)                                             1 byte  |
    ===============================================================================
    */

    using byte = uint8_t;

    // READ TYPE
    byte type = file_data[cursor];
    cursor++;

    // READ INFO
    bool is_required = (file_data[cursor] & (1 << 0)) == 1;
    bool is_ordered = (file_data[cursor] & (1 << 1)) == 1;
    cursor++;

    // READ NAME
    byte namesize = file_data[cursor];
    cursor++;
    std::string name;
    for (int i = 0; i < namesize; i++) {
        name += static_cast<char>(file_data[cursor]);
        cursor++;
    }

    // READ VARNAME
    byte varnamesize = file_data[cursor];
    cursor++;
    std::string varname;
    for (int i = 0; i < varnamesize; i++) {
        varname += static_cast<char>(file_data[cursor]);
        cursor++;
    }

    // READ CHILDREN
    byte childcount = file_data[cursor];
    cursor++;
    std::unique_ptr<Object_Block> block;
    if (childcount > 0) {
        block = std::make_unique<Object_Block>(std::vector<std::unique_ptr<Object>>{});
        for (int i = 0; i < childcount; i++) {
            block->children.push_back(readObject());
        }
    }
    if (block)
        block->is_ordered = is_ordered;

    // CONSTRUCT AND RETURN NODE
    std::unique_ptr<Object> node;
    switch (type) {
        case uint8_t(0):
            throw std::runtime_error("invalid object type 0 during reading at position " +
                                     std::to_string(cursor));
            break;
        case uint8_t(1):
            node = std::move(block);
            break;
        case uint8_t(2):
            node = std::make_unique<Object_Arg>(varname, is_required, std::move(block));
            break;
        case uint8_t(3):
            node = std::make_unique<Object_Flag>(varname, name, is_required, std::move(block));
            break;
        case uint8_t(4):
            node = std::make_unique<Object_Val>(varname, name, is_required, std::move(block));
            break;
        default:
            break;
    }

    return std::move(node);
}
void argwc::read_config() {
    while (cursor < file_data.size()) {
        entry_point->children.push_back(readObject());
    }
}
void argwc::read_arguments() {
    std::unordered_set<std::string> provided_args;
    for (int i = 1; i < argc; ++i) { // 1 is the program name, skip it
        provided_args.insert(std::string(argv[i]));
    }

    // we'll build active object list starting from top-level Objects, when a flag
    // with an if_passed block is present in rhe args, its block children become active too
    std::vector<Object*> active_objs;
    active_objs.reserve(entry_point->children.size());
    for (auto& obj : entry_point->children) {
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
    for (auto& obj : entry_point->children) {
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