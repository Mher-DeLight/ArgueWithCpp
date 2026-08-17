#include "argfile.h" // if this line errors for you, you probably didn't generate the argfile. see the README.
#include <argwc.h>
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv, argfile::data);

    if (mngr.arg_exists("direc"))
        std::cout << mngr.get_arg("direc") << std::endl;
    else
        std::cout << "no path provided" << std::endl;
}