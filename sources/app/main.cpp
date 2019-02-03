#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#include <CommandLineHelpers.hpp>
#include <Cpp2JsonActionFactory.hpp>
#include <Cpp2JsonParameters.hpp>
#include "JsonOutputWriter.hpp"
#include <rapidjson/istreamwrapper.h>

#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include <iostream>


static llvm::cl::OptionCategory cpp2JsonCategory("Main options");

static llvm::cl::opt<std::string> outputFile("output",
                                                   llvm::cl::init("-"),
                                                   llvm::cl::desc("Specify output JSON filename.\n"
                                                                  "You can also send the output to the stdout using - instead of a filename"),
                                                   llvm::cl::value_desc("filename"),
                                                   llvm::cl::cat(cpp2JsonCategory));
static llvm::cl::opt<bool> appendOutput("append",
                                        llvm::cl::init(false),
                                        llvm::cl::desc("Append output"),
                                        llvm::cl::cat(cpp2JsonCategory));

static llvm::cl::opt<bool> prettyOutput("pretty",
                                        llvm::cl::init(false),
                                        llvm::cl::desc("Output pretty JSON"),
                                        llvm::cl::cat(cpp2JsonCategory));

static llvm::cl::opt<std::string> excludeAnnotation("exclude_annotation",
                                                   llvm::cl::init("Exclude_cpp2json"),
                                                   llvm::cl::desc("Specify the content for exclude annotation"),
                                                   llvm::cl::value_desc("annotation content"),
                                                   llvm::cl::cat(cpp2JsonCategory));

static void readJsonDocumentFromFile(rapidjson::Document& document, std::string const& filePath)
{
    std::ifstream file(filePath);

    if (file.is_open())
    {
        rapidjson::IStreamWrapper isw(file);

        document.ParseStream(isw);
    }
}

/*!
 * \brief Create and fill an instance of Cpp2JsonParameters using
 * the command line arguments options.
 */
static Cpp2JsonParameters makeParameters()
{
    Cpp2JsonParameters parameters;

    parameters.outputFilePath = outputFile.getValue();
    parameters.prettyOutput = prettyOutput.getValue();
    parameters.appendOutput = appendOutput.getValue();
    parameters.excludeAnnotationContent = excludeAnnotation.getValue();
    return parameters;
}

int main(int argc, char const** argv)
{
    clang::tooling::CommonOptionsParser optionParser(argc, argv, cpp2JsonCategory);
    clang::tooling::ClangTool tool(optionParser.getCompilations(), optionParser.getSourcePathList());
    clang::tooling::CommandLineArguments extraArguments = makeLlvmIncludeArguments();
    auto extra = clang::tooling::getInsertArgumentAdjuster(extraArguments, clang::tooling::ArgumentInsertPosition::END);
    rapidjson::Document jsonDocument(rapidjson::Type::kObjectType);
    auto const parameters = makeParameters();

    if (parameters.appendOutput)
    {
        readJsonDocumentFromFile(jsonDocument, parameters.outputFilePath);
    }
    tool.appendArgumentsAdjuster(extra);

    auto const frontEndActionFactory = newCpp2JsonActionFactory(parameters, jsonDocument);
    auto const result = tool.run(frontEndActionFactory.get());

    if (result == 0)
    {
        JsonOutputWriter const writer(parameters);

        writer.write(jsonDocument);
    }
    return result;
}
