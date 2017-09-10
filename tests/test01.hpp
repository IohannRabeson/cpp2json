#ifndef TEST01_HPP
#define TEST01_HPP
#include <string>
#include <vector>
#include <unordered_set>

#include "../shared/Cpp2JsonAnnotations.hpp"

namespace burp
{
    struct __attribute__((annotate("yolo"))) __attribute__((annotate("gerialize"))) Test
    {
        int hello;
        std::string string;
        double d;
        float yolo;
        std::vector<std::string> yup;
        std::unordered_set<std::string> yop;
        std::string anArray[2];
    };
}

struct __attribute__((annotate("yolo"))) CPP2JSON_EXCLUDE Test2NotParsed
{
    int hello;
    std::string string;
    double d;
    float yolo;
    std::vector<std::string> yup;
};

#endif // TEST01_HPP
