#include <argwc.h>
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv);

    mngr.print_arguments();
    if (mngr.flag_enabled("verbose")) {
        std::cout << "I AM BEING VERY VERBOSE" << std::endl;
    }
}