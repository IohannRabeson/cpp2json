#ifndef CPP2JSONVISITOR_HPP
#define CPP2JSONVISITOR_HPP
#include <clang/AST/RecursiveASTVisitor.h>
#include <rapidjson/document.h>

struct Cpp2JsonParameters;

class Cpp2JsonVisitor : public clang::RecursiveASTVisitor<Cpp2JsonVisitor>
{
public:
    explicit Cpp2JsonVisitor(Cpp2JsonParameters const& parameters, rapidjson::Document& jsonDocument);

    void addTypeNameReplacer(std::string const& original, std::string const& replacement);

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
    void parseType(clang::QualType const& type, rapidjson::Value& root, std::string const& jsonKey, clang::ASTContext& context) const;

    bool isExcludedDeclaration(clang::CXXRecordDecl const* declaration)const;
    bool isExcludedDeclaration(clang::EnumDecl const* declaration)const;
    bool isExcludedDeclaration(clang::CXXMethodDecl const* declaration)const;
    bool hasExcludeAnnotation(clang::Decl const* declaration)const;

    void addOrReplaceJsonMember(rapidjson::Value::Object& object, std::string const& key, rapidjson::Value &value);

    std::string getNormalizedTypeString(clang::QualType const& qualType) const;
    std::string getCleanedTypeString(clang::QualType const& qualType) const;
    std::string replaceTypeNames(std::string typeName) const;
    std::string removeClassStructEnum(std::string typeName) const;
private:
    std::map<std::string, std::string> m_typeNameReplacer;
    Cpp2JsonParameters const& m_parameters;
    rapidjson::Document& m_jsonDocument;
    rapidjson::Document::AllocatorType& m_jsonAllocator;
    rapidjson::Value::Object m_jsonClasses;
    rapidjson::Value::Object m_jsonEnums;
};

#endif // CPP2JSONVISITOR_HPP
