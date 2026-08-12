# ArgueWithCpp
## What is ArgueWithCpp?
ArgueWithCpp (`argwc`) is a C++ library designed to make argument parsing for C++ programs easier and more convenient using config files ("Grammar files"). Those files describe the structure of the arguments and how they should be passed, and then they could be used in the code directly with no extra overhead.
## Example Snipper
Here's an example snippet of the `ArgGrammar.awc` file:
```
argfile {
    buzzword "--dir" {
        arg $path required;
    };
    flag $verbose "--verbose";
};
```
And to use in C++:
```cpp
#include <argwc.h>
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv);

    if (mngr.arg_exists("path"))
        std::cout << mngr.get_arg("path") << std::endl;
    else
        std::cout << "no path provided" << std::endl;
    
    if (mngr.flag_enabled("verbose")) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
}
```
Then the program can be ran via:
```bash
./myprogram --dir sometextgoeshere --verbose
```