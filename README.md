# Cpp2Json [![Build Status](https://travis-ci.com/IohannRabeson/cpp2json.svg?token=oSgYDG8ZHmxB1gxGNZxP&branch=master)](https://travis-ci.com/IohannRabeson/cpp2json) 

A tool to extract C++ classes informations about methods and attributes.  
Idea from Sergiy Migdalskiy: https://archive.org/details/GDC2014Migdalskiy

## Example
```c++
#ifndef TEST03_HPP
#define TEST03_HPP

namespace burk
{
    enum class YoloEnum
    {
        Caca = -1,
        Pipi = -2,
        Vomi = -3
    };
}

enum YalaEnum
{
    Ouf,
    Yop,
    Hop
};

template <class T>
struct Test8
{
    T templateField;
    const bool justForFun;
    volatile burk::YoloEnum degueu;
    mutable YalaEnum yalala;
};

#endif // TEST03_HPP
```

```bash
$> cpp2json test03.hpp -pretty -- -std=c++14
```

```json
{
    "classes": [
        {
            "className": "Test8",
            "fields": [
                {
                    "fieldName": "templateField",
                    "fieldType": "T"
                },
                {
                    "fieldName": "justForFun",
                    "fieldType": "_Bool"
                },
                {
                    "fieldName": "degueu",
                    "fieldType": "burk::YoloEnum"
                },
                {
                    "fieldName": "yalala",
                    "fieldType": "enum YalaEnum"
                }
            ],
            "templateParameters": [
                "T"
            ]
        }
    ],
    "enums": [
        {
            "enumName": "burk::YoloEnum",
            "underlyingType": "int",
            "values": {
                "Caca": -1,
                "Pipi": -2,
                "Vomi": -3
            }
        },
        {
            "enumName": "YalaEnum",
            "underlyingType": "unsigned int",
            "values": {
                "Ouf": 0,
                "Yop": 1,
                "Hop": 2
            }
        }
    ]
}
```
## Usage
```bash
  cpp2json [options] <source0> [... <sourceN>] -- [clang options]

  -output=<filename>                       - Specify output JSON filename.
                                             You can also send the output to the stdout using - instead of a filename
  -pretty                                  - Output pretty JSON
  -exclude_annotation=<annotation content> - Specify the content for exclude annotation.
```
Note the --, it's really needed because this flag define where the options passed to the clang driver begins.  
If cpp2json can't find standard includes (it can happen if you have moved cpp2json in another location), you should specify
yourself using the correct llvm include paths using -I (after the --).

Don't forget to specify the c++ version used too as clang option (e.g: -std=c++14).
Example:
```
  cpp2json [options] <source0> [... <sourceN>] -- -std=c++14
```
This is required because clang assume the executable is located in a standard directory (in llvm/bin) and use that
to deduce the includes paths.

## How to build
```bash
   $> mkdir builds && cd builds
   $> cmake .. -G Ninja -DCMAKE_PREFIX_PATH=<your_path_to_llvm_install>
   $> ninja
```
If you need to set more than one path to CMAKE_PREFIX_PATH separate each path by a semi-colon ';'. You should encloses the paths using double quotes ('`"`')
```bash
   $> cmake .. -G Ninja -DCMAKE_PREFIX_PATH="<llvm_path>;<qt_path>"
```
