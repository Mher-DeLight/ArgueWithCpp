#include "argfile.h" // if this line errors for you, you probably didn't generate the argfile. see the README.
#include <argwc.h>
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv, argfile::data);

    std::cout << mngr.get_arg("dir") << std::endl;
}