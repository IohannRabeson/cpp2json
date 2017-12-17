#ifndef CPP2JSONPARAMETERS_HPP
#define CPP2JSONPARAMETERS_HPP

#include <string>

struct Cpp2JsonParameters
{
    Cpp2JsonParameters() :
        prettyOutput(false),
        appendOutput(false)
    {
    }

    std::string outputFilePath;
    std::string excludeAnnotationContent;
    bool prettyOutput;
    bool appendOutput;
};

#endif // CPP2JSONPARAMETERS_HPP
