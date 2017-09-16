#ifndef CPP2JSONVISITOR_HPP
#define CPP2JSONVISITOR_HPP
#include <clang/AST/RecursiveASTVisitor.h>
#include <rapidjson/document.h>

struct Cpp2JsonParameters;

class Cpp2JsonVisitor : public clang::RecursiveASTVisitor<Cpp2JsonVisitor>
{
public:
    explicit Cpp2JsonVisitor(rapidjson::Document& jsonDocument);

    bool VisitEnumDecl(clang::EnumDecl* declaration);
    bool VisitCXXRecordDecl(clang::CXXRecordDecl* declaration);
private:
    void parseClass(clang::CXXRecordDecl *classDeclaration);
    void parseEnum(clang::EnumDecl* enumDeclaration);

    void parseAnnotations(clang::Decl const* classDeclaration, rapidjson::Value& jsonClassObject);
    void parseMethod(clang::CXXMethodDecl const* methodDeclaration, rapidjson::Value::Array& jsonMethodArray);
    void parseMethods(clang::CXXRecordDecl* classDeclaration, rapidjson::Value& jsonClassObject);
    void parseFunctionParameters(clang::FunctionDecl const* functionDecl, rapidjson::Value::Array& jsonParameterArray);
    void parseField(clang::FieldDecl* fieldDeclaration, rapidjson::Value& jsonFieldArray);
    void parseFields(clang::CXXRecordDecl* classDeclaration, rapidjson::Value& jsonClassObject);
    void parseBaseClasses(clang::CXXRecordDecl* classDeclaration, rapidjson::Value& jsonClassObject);
    void parseClassTemplateParameters(clang::CXXRecordDecl* classDeclaration, rapidjson::Value& jsonClassObject);
    void parseIncludeDeclaration(clang::Decl* declaration, rapidjson::Value& jsonObject);

    bool isExcludedDeclaration(clang::CXXRecordDecl const* declaration)const;
    bool isExcludedDeclaration(clang::EnumDecl const* declaration)const;
    bool isExcludedDeclaration(clang::FieldDecl const* declaration)const;
    bool isExcludedDeclaration(clang::CXXMethodDecl const* declaration)const;
private:
    void addOrReplaceJsonMember(rapidjson::Value::Object& object, std::string const& key, rapidjson::Value &value);
private:
    static std::string const ExcludeParsingAnnotation;

    rapidjson::Document& m_jsonDocument;
    rapidjson::Document::AllocatorType& m_jsonAllocator;
    rapidjson::Value::Object m_jsonClasses;
    rapidjson::Value::Object m_jsonEnums;
};

#endif // CPP2JSONVISITOR_HPP
