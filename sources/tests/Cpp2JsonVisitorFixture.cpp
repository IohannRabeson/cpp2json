#include "Cpp2JsonVisitorFixture.hpp"

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#include <Cpp2JsonParameters.hpp>
#include <Cpp2JsonActionFactory.hpp>

static std::string const LlvmBasePath{LLVM_DIR};
static std::string const ResourcesPath{RESOURCE_DIR};

static clang::tooling::CommandLineArguments makeLlvmIncludeArguments()
{
    std::string const llvmVersion = LlvmBasePath.substr(LlvmBasePath.find_last_of('/'));
    clang::tooling::CommandLineArguments extraArguments{"-I" + LlvmBasePath + "/include/c++/v1",
                                                        "-I" + LlvmBasePath + "/lib/clang/" + llvmVersion + "/include"};

    extraArguments.push_back("-std=c++14");
    return extraArguments;
}

static Cpp2JsonParameters makeParameters()
{
    Cpp2JsonParameters parameters;

    parameters.outputFilePath = "-";
    parameters.prettyOutput = true;
    return parameters;
}

void Cpp2JsonVisitorFixture::SetUp()
{
    parseCpp(ResourcesPath + "/test_type_qualifier.hpp");
}

void Cpp2JsonVisitorFixture::parseCpp(const std::string &path)
{
    static llvm::cl::OptionCategory cpp2JsonCategory("Main options");
    std::vector<char const*> argv{"", path.c_str(), "--", "-std=c++14"};
    int argc = static_cast<int>(argv.size()) - 1;

    clang::tooling::CommonOptionsParser optionParser(argc, argv.data(), cpp2JsonCategory);
    clang::tooling::ClangTool tool(optionParser.getCompilations(), optionParser.getSourcePathList());
    clang::tooling::CommandLineArguments extraArguments = makeLlvmIncludeArguments();
    auto extra = clang::tooling::getInsertArgumentAdjuster(extraArguments, clang::tooling::ArgumentInsertPosition::END);
    rapidjson::Document jsonDocument(rapidjson::Type::kObjectType);
    auto const parameters = makeParameters();

    tool.appendArgumentsAdjuster(extra);

    auto const frontEndActionFactory = newCpp2JsonActionFactory(parameters, m_document);
    auto const result = tool.run(frontEndActionFactory.get());

    ASSERT_EQ (result, 0);
}
