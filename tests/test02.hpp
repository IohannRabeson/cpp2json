#ifndef TEST02_HPP
#define TEST02_HPP
#include <string>
#include <vector>
#include <unordered_set>

#include "../shared/Cpp2JsonAnnotations.hpp"

namespace burp
{
    struct Test3
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

struct Test4 : public burp::Test3
{
    int hello;
    std::string string;
    double d;
    float yolo;
    std::vector<std::string> yup;
};

struct Test5 : Test4
{
    int hello;
    std::string string;
    double d;
    float yolo;
    CPP2JSON_EXCLUDE std::vector<std::string> notParsed;
};

struct Test6
{
    std::string mmmm;
};

struct Test7 : Test6, burp::Test3
{
};

#endif // TEST02_HPP
