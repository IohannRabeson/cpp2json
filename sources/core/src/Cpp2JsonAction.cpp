#include "Cpp2JsonAction.hpp"
#include "Cpp2JsonVisitor.hpp"

#include <clang/Frontend/CompilerInstance.h>

#include <iostream>

class Cpp2JsonConsumer : public clang::ASTConsumer
{
public:
    explicit Cpp2JsonConsumer(Cpp2JsonParameters const& parameters, rapidjson::Document& jsonDocument) :
        m_visitor(parameters, jsonDocument)
    {
    }

    void HandleTranslationUnit(clang::ASTContext& context) override
    {
        auto* const declaration = context.getTranslationUnitDecl();

        m_visitor.TraverseDecl(declaration);
    }
private:
    Cpp2JsonVisitor m_visitor;
};

Cpp2JsonAction::Cpp2JsonAction(Cpp2JsonParameters const& parameters, rapidjson::Document& jsonDocument) :
    m_parameters(parameters),
    m_jsonDocument(jsonDocument)
{
}

std::unique_ptr<clang::ASTConsumer> Cpp2JsonAction::CreateASTConsumer(clang::CompilerInstance& compilerInstance, llvm::StringRef inFile)
{
    return std::unique_ptr<clang::ASTConsumer>(new Cpp2JsonConsumer(m_parameters, m_jsonDocument));
}