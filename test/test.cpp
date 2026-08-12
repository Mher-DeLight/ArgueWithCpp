#include <argwc.h>
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "== ARGS ==\n";
    for (int i = 0; i < argc; i++) {
        std::cout << std::to_string(i + 1) << ") ";
        std::cout << argv[i] << std::endl;
    }
    std::cout << std::endl;
    argwc mngr(argc, argv);
    if (mngr.get_flag_value("verbose")) {
        std::cout << "I AM BEING VERY VERBOSE" << std::endl;
    }
}