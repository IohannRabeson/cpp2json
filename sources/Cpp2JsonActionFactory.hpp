#ifndef CPP2JSONACTIONFACTORY_HPP
#define CPP2JSONACTIONFACTORY_HPP
#include "Cpp2JsonParameters.hpp"

#include <clang/Tooling/Tooling.h>
#include <rapidjson/document.h>

class Cpp2JsonActionFactory : public clang::tooling::FrontendActionFactory
{
public:
    explicit Cpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document);

    clang::FrontendAction *create() override;
private:
    rapidjson::Document& m_jsonDocument;
    Cpp2JsonParameters const& m_parameters;
};

std::unique_ptr<Cpp2JsonActionFactory> newCpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document);

#endif // CPP2JSONACTIONFACTORY_HPP
