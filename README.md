# Cpp2Json [![Build Status](https://travis-ci.com/IohannRabeson/cpp2json.svg?token=oSgYDG8ZHmxB1gxGNZxP&branch=master)](https://travis-ci.com/IohannRabeson/cpp2json) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/456265ad762c40d5aca25cb649589c69)](https://www.codacy.com/app/IohannRabeson/cpp2json?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=IohannRabeson/cpp2json&amp;utm_campaign=Badge_Grade)

![general_usage](https://docs.google.com/drawings/d/e/2PACX-1vSvz2O3s9Oica9CyYAUq42n7U62ygswZS-CPsdTIPUDFLky-4Ff0zx-U-kBqJSubkmiBsJ9GsByTDsl/pub?w=1149&h=542)

A tool to extract C++ information about enumerations, classes, methods and attributes.  
All theses data are stored as JSON.

Initial idea picked up from this conference by Sergiy Migdalskiy: https://archive.org/details/GDC2014Migdalskiy

## Requirements

- MacOS
- Llvm 11 - Llvm 12 is not supported
- RapidJson 1.x

You might need to give the path to the llvm folder to CMake:
```bash
brew install llvm@11 rapidjson ninja
mkdir builds && cd builds
cmake .. -G Ninja -DCMAKE_PREFIX_PATH=$(brew --prefix llvm@11)
ninja
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
Note the -- is really needed because to define where the options passed to the clang driver begins.  
If cpp2json can't find standard includes (it can happen if you have moved cpp2json in another location), you should specify
yourself using the correct llvm include paths using `-I` (after the --).

Don't forget to specify the c++ version used as clang option (e.g: -std=c++14).
Example:
```
  cpp2json [options] <source0> [... <sourceN>] -- -std=c++14
```
This is required because clang assume the executable is located in a standard directory (in llvm/bin) and use that
to deduce the includes paths.