#include <argwc.h>
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv);

    mngr.print_arguments();
    std::cout << mngr.get_arg("mypath") << std::endl;
}