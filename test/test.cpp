#include <argwc.h>
#include "argfile.h" // if this line errors for you, you probably didn't generate the argfile. see the README.
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv, argfile().text);

    if (mngr.arg_exists("path"))
        std::cout << mngr.get_arg("path") << std::endl;
    else
        std::cout << "no path provided" << std::endl;
}