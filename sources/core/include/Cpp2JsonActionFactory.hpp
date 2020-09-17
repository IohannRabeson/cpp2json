#ifndef CPP2JSON_ACTION_FACTORY_HPP
#define CPP2JSON_ACTION_FACTORY_HPP
#include <clang/Tooling/Tooling.h>
#include <rapidjson/document.h>

struct Cpp2JsonParameters;

class Cpp2JsonActionFactory : public clang::tooling::FrontendActionFactory
{
public:
    explicit Cpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document);

    std::unique_ptr<clang::FrontendAction> create() override;
private:
    rapidjson::Document& m_jsonDocument;
    Cpp2JsonParameters const& m_parameters;
};

std::unique_ptr<Cpp2JsonActionFactory> newCpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document);

#endif // CPP2JSON_ACTION_FACTORY_HPP
