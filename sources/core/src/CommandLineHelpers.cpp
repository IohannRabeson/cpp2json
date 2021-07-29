#include "CommandLineHelpers.hpp"

static std::string const LlvmBasePath{LLVM_DIR};

clang::tooling::CommandLineArguments makeLlvmIncludeArguments()
{
    assert( !LlvmBasePath.empty() );

    std::string const llvmVersion = LlvmBasePath.substr(LlvmBasePath.find_last_of('/'));
    clang::tooling::CommandLineArguments extraArguments{
        "-std=c++14",
        "-I" + LlvmBasePath + "/include/c++/v1",
        "-I" + LlvmBasePath + "/lib/clang/" + llvmVersion + "/include",
    };

    return extraArguments;
}
