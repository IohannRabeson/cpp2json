#ifndef TEST03_HPP
#define TEST03_HPP
#include <string>
#include <vector>
#include <unordered_set>

namespace burk
{
    enum YoloEnum
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
struct __attribute__((annotate("yep"))) Test8
{
    T templateField;
    const bool justForFun;
    volatile burk::YoloEnum degueu;
    mutable YalaEnum yalala;
    int array_of_ints[123];
    int const const_array_of_ints[123];

    void yep()
    {
    }

    std::string yep(std::string const& hello, int yala)const
    {
        return "";
    }
};

struct Test9
{
    Test9()
    {
    }

    ~Test9()
    {
    }

    Test9& operator = (Test9 const&)
    {
        return *this;
    }
};

#endif // TEST03_HPP
