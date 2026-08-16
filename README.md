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
#include "argfile.h"
#include <iostream>

int main(int argc, char** argv) {
    argwc mngr(argc, argv, argfile().text);

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
## Todo
- [ ] Add custom string formatting for `val` and `arg` (perhaps via regex?)
- [ ] Make `ordered` actually work
- [ ] Reimplement the parse args function (i can barely understand it)
- [ ] Incorporate it with argwc-comp such that they don't have to be separate executables
- [ ] Custom error messages assigned by the user
- [ ] Syntax highlighter for VSCode
- [ ] Macros?
- [ ] Add an optional `xargs` attribute to `argfile` such that text from stdin is accepted as argument
## Documentation
### C++
In the C++ half, `argwc` is invoked like this:
```cpp
#include <ArgueWithCpp/argwc.h>
#include "argfile.h"

int main(int argc, char** argv) {
    argwc argmn(argc, argv, argfile().text);
   // use argwc here
}
```
#### Functions
```cpp
argwc(int argc, char** argv, const std::string& code)
```
The class constructor. Takes the `argc` and `argv` parameters passed to the `main()` function. Also takes `code` arguement which should be included from the argfile. See [Argue With Cpp Compiler](#arguewithcpp-compiler-argwc-comp).
```cpp
bool flag_enabled(const std::string& flagname)
```
Returns true if the `flag` is present, `false` otherwise.
```cpp
std::string get_arg(const std::string& varname)
``` 
Returns the value of the argument `varname`.
```cpp
void print_arguments()
``` 
Prints the arguments passed to argwc, excluding the program path/name.
```cpp
bool flag_exists(const std::string& flagname);
bool arg_exists(const std::string& argname);
bool obj_exists(const std::string& objname);
```
Each return `true` if the corresponding object is present, while `obj_exists` returns true if an object from either type is present. All return `false` otherwise.
### ArgueWithCpp
In the ArgueWithCpp half, you must create an `ArgGrammar.awc` file that describes the arguments grammar. Every Grammar file starts with a:
```
argfile {
    // grammar goes here
};
```
Every statement ends with a semicolon. As of now, there are three statements, `buzzword`, `flag`, and `arg` statements. `buzzword` statements are arguments consumed solely for aesthetics or structure. `flag` are statements that return `true` if present and `false` otherwise. And `arg` may contain an arbitrary string. Here's an example:
```
argfile {
    buzzword "--greet" required {
        arg $name required;
    }
    flag $beFriendly "--be-friendly";
};
```
The brackets after a statement but before its semicolon represents a scope where the statements inside will only be invoked if the outer statement is also passed. Statements marked with `required` will make the program throw if not passed.
### ArgueWithCpp Compiler (argwc-comp)
The ArgueWithCpp compiler is a arwc-to-C++ transpiler to allow packacing the Grammar file directly with the executable. [Download it from here.](https://github.com/Mher-DeLight/ArgwcCompiler/releases) Then put it in your `~/.bin`, `~/.local/bin`, or if you have root access, `/usr/local/bin`. Then write your ArgueWithCpp file and compile it with `argwc-comp /path/to/your/grammarfile.awc /path/to/your/src/directory/argfile.h`. Then include it from the C++ file and use it as [above](#example-snipper).
