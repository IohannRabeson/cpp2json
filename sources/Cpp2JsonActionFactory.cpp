#include "Cpp2JsonActionFactory.hpp"
#include "Cpp2JsonAction.hpp"

Cpp2JsonActionFactory::Cpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document) :
    m_jsonDocument(document),
    m_parameters(parameters)
{
}

clang::FrontendAction *Cpp2JsonActionFactory::create()
{
    return new Cpp2JsonAction(m_parameters, m_jsonDocument);
}

std::unique_ptr<Cpp2JsonActionFactory> newCpp2JsonActionFactory(Cpp2JsonParameters const& parameters, rapidjson::Document& document)
{
    return std::unique_ptr<Cpp2JsonActionFactory>(new Cpp2JsonActionFactory(parameters, document));
}
