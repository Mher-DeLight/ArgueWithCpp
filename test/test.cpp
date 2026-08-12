#include <argwc.h>
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv);

    if (mngr.arg_exists("path"))
        std::cout << mngr.get_arg("path") << std::endl;
    else
        std::cout << "no path provided" << std::endl;
}