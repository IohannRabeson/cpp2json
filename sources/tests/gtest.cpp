#include <gtest/gtest.h>
#include "Cpp2JsonVisitorFixture.hpp"

// TODO: Cpp2JsonVisitorFixture en utilisant le main de l'application.
// En particulier on a juste besoin des extra arguments:
//
//      clang::tooling::CommandLineArguments extraArguments = makeLlvmIncludeArguments();
//      auto extra = clang::tooling::getInsertArgumentAdjuster(extraArguments, clang::tooling::ArgumentInsertPosition::END);
//

TEST_F(Cpp2JsonVisitorFixture, Qualifiers)
{

}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
