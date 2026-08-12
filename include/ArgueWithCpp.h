#pragma once

class arguewc {
private:
    struct ArgInfo {
    public:
        char* value;
        bool required;
        bool is_free;

        ArgInfo(char* value_, bool required_ = false, bool is_free_ = false)
            : value(value_), required(required_), is_free(is_free_) {}
    };

    int argc;
    char** argv;
    ArgInfo* valid_args;
    int valid_arg_count;

public:
    /// @brief If true, allows all arguments, removing the need to explicitly tell arguewc to allow
    /// it.
    bool allow_all_arguments = false;

    arguewc(int argc_, char** argv_) : argc(argc_), argv(argv_) {}
};