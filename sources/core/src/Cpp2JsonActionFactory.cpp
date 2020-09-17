#include "Cpp2JsonActionFactory.hpp"
#include "Cpp2JsonAction.hpp"
#include "Cpp2JsonParameters.hpp"

Cpp2JsonActionFactory::Cpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document) :
    m_jsonDocument(document),
    m_parameters(parameters)
{
}

std::unique_ptr<clang::FrontendAction> Cpp2JsonActionFactory::create()
{
    return std::make_unique<Cpp2JsonAction>(m_parameters, m_jsonDocument);
}

std::unique_ptr<Cpp2JsonActionFactory> newCpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document)
{
    return std::unique_ptr<Cpp2JsonActionFactory>(new Cpp2JsonActionFactory(parameters, document));
}
