#ifndef CPP2JSONACTION_HPP
#define CPP2JSONACTION_HPP
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <rapidjson/document.h>

struct Cpp2JsonParameters;

class Cpp2JsonAction : public clang::ASTFrontendAction
{
public:
    explicit Cpp2JsonAction(Cpp2JsonParameters const& parameters, rapidjson::Document& jsonDocument);
protected:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compilerInstance, llvm::StringRef inFile) override;
private:
    Cpp2JsonParameters const& m_parameters;
    rapidjson::Document& m_jsonDocument;
};

#endif // CPP2JSONACTION_HPP
