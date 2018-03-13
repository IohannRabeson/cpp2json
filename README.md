# Cpp2Json [![Build Status](https://travis-ci.com/IohannRabeson/cpp2json.svg?token=oSgYDG8ZHmxB1gxGNZxP&branch=master)](https://travis-ci.com/IohannRabeson/cpp2json) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/456265ad762c40d5aca25cb649589c69)](https://www.codacy.com/app/IohannRabeson/cpp2json?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=IohannRabeson/cpp2json&amp;utm_campaign=Badge_Grade)

![general_usage](https://docs.google.com/drawings/d/e/2PACX-1vSvz2O3s9Oica9CyYAUq42n7U62ygswZS-CPsdTIPUDFLky-4Ff0zx-U-kBqJSubkmiBsJ9GsByTDsl/pub?w=1149&h=542)


A tool to extract C++ information about enumerations, classes, methods and attributes.  
All theses data are stored as JSON.

Initial idea picked up from this conference by Sergiy Migdalskiy: https://archive.org/details/GDC2014Migdalskiy

## Example
File test_example.hpp:
```c++
#ifndef CPP2JSON_TEST_EXAMPLE_HPP
#define CPP2JSON_TEST_EXAMPLE_HPP
#include <string>

struct Example01
{
    float m_float;
    std::string m_string;
    int m_int;
};

#endif //CPP2JSON_TEST_EXAMPLE_HPP
```

Run cpp2json on it:
```bash
$> cpp2json test_example.hpp --pretty --
```

Will give you the following JSON output:
```json
{
    "classes": {
        "Example01": {
            "fields": [
                {
                    "name": "m_float",
                    "type": {
                        "key": "float",
                        "expression": "float",
                        "pointer": false,
                        "array": false,
                        "reference": false,
                        "const": false,
                        "volatile": false,
                        "literal": true,
                        "enum": false
                    }
                },
                {
                    "name": "m_string",
                    "type": {
                        "key": "std::string",
                        "expression": "std::string",
                        "pointer": false,
                        "array": false,
                        "reference": false,
                        "const": false,
                        "volatile": false,
                        "literal": false,
                        "enum": false
                    }
                },
                {
                    "name": "m_int",
                    "type": {
                        "key": "int",
                        "expression": "int",
                        "pointer": false,
                        "array": false,
                        "reference": false,
                        "const": false,
                        "volatile": false,
                        "literal": true,
                        "enum": false
                    }
                }
            ],
            "methods": [],
            "annotations": [],
            "bases": [],
            "file": "/Users/iota/projects/tools/cpp2json/sources/tests/resources/test_example.hpp"
        }
    },
    "enums": {}
}
```
## Usage
```bash
USAGE: cpp2json [options] <source0> [... <sourceN>]

OPTIONS:

Generic Options:

  -help                                    - Display available options (-help-hidden for more)
  -help-list                               - Display list of available options (-help-list-hidden for more)
  -version                                 - Display the version of this program

Main options:

  -append                                  - Append output
  -exclude_annotation=<annotation content> - Specify the content for exclude annotation
  -extra-arg=<string>                      - Additional argument to append to the compiler command line
  -extra-arg-before=<string>               - Additional argument to prepend to the compiler command line
  -output=<filename>                       - Specify output JSON filename.
                                             You can also send the output to the stdout using - instead of a filename
  -p=<string>                              - Build path
  -pretty                                  - Output pretty JSON
```
Note the --, it's really needed because this flag define where the options passed to the clang driver begins.  
If cpp2json can't find standard includes (it can happen if you have moved cpp2json in another location), you should specify
yourself using the correct llvm include paths using `-I` (after the --).

Don't forget to specify the c++ version used as clang option (e.g: -std=c++14).
Example:
```
  cpp2json [options] <source0> [... <sourceN>] -- -std=c++14
```
This is required because clang assume the executable is located in a standard directory (in llvm/bin) and use that
to deduce the includes paths.

## How to clone
You should clone git submodules if you want to build tests:
```bash
    $> git clone --recursive git@github.com:IohannRabeson/cpp2json.git
```

## How to build
You should generate the project out of the sources directory.  
```bash
   $> mkdir builds && cd builds
   $> cmake .. -G Ninja -DCMAKE_PREFIX_PATH=<your_path_to_llvm_install>
   $> ninja
```
If you need to set more than one path to CMAKE_PREFIX_PATH separate each path by a semi-colon ';'. You should encloses the paths using double quotes ('`"`') since `;` is a special character on most shell.
```bash
   $> cmake .. -G Ninja -DCMAKE_PREFIX_PATH="<llvm_path>;<qt_path>"
```

### Building with OSX
On OSX hopefully we can use [Homebrew](https://brew.sh/index_fr.html) to install all dependencies.  
This can be done with only one command:
```bash
    $> brew install llvm
```
And it's done. Use CMake to generate the project:
```bash
   $> cmake .. -G Ninja -DCMAKE_PREFIX_PATH="/usr/local/opt/llvm;/usr/local/opt/qt5"
```

Finally build:
```bash
    $> cmake --build .
```
