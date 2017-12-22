#include "Cpp2JsonVisitorFixture.hpp"

#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#include <Cpp2JsonParameters.hpp>
#include <Cpp2JsonActionFactory.hpp>
#include <JsonOutputWriter.hpp>
#include <CommandLineHelpers.hpp>

static Cpp2JsonParameters makeParameters()
{
    Cpp2JsonParameters parameters;

    parameters.outputFilePath = "-";
    parameters.prettyOutput = true;
    return parameters;
}

Cpp2JsonVisitorFixture::Cpp2JsonVisitorFixture() :
    m_document(rapidjson::Type::kObjectType)
{
}

std::string Cpp2JsonVisitorFixture::getCpp2JsonVersion() const
{
    // TODO: store "cpp2json_format" into a variable.
    return m_document["cpp2json_format"].GetString();
}

rapidjson::Value::ConstObject Cpp2JsonVisitorFixture::getEnums() const
{
    // TODO: store "enums" into a variable.
    return m_document["enums"].GetObject();
}

rapidjson::Value::ConstObject Cpp2JsonVisitorFixture::getClasses() const
{
    // TODO: store "classes" into a variable.
    return m_document["classes"].GetObject();
}

rapidjson::Value::ConstObject Cpp2JsonVisitorFixture::getField(std::string const& className, std::string const& fieldName) const
{
    auto const fields = getFieldsOf(className);

    for (auto it = fields.Begin(); it != fields.End(); ++it)
    {
        auto const field = it->GetObject();

        if (field.HasMember("name") && field["name"].IsString() && field["name"].GetString() == fieldName)
        {
            return field;
        }
    }
    assert(false);
}

rapidjson::Value::ConstArray Cpp2JsonVisitorFixture::getFieldsOf(const std::string &className) const
{
    // TODO: store "classes" into a variable.
    rapidjson::Value::ConstObject classes = m_document["classes"].GetObject();

    if (!classes.HasMember(className.c_str()))
        ADD_FAILURE() << "Unable to find class '" << className << "'";

    rapidjson::Value::ConstObject members = classes[className.c_str()].GetObject();

    if (!members.HasMember("fields"))
        ADD_FAILURE() << "Unable to find 'fields' member in class '" << className << "'";

    return members["fields"].GetArray();
}

rapidjson::Value::ConstArray Cpp2JsonVisitorFixture::getMethodsOf(const std::string &className) const
{
    // TODO: store "classes" into a variable.
    rapidjson::Value::ConstObject classes = m_document["classes"].GetObject();

    if (!classes.HasMember(className.c_str()))
        ADD_FAILURE() << "Unable to find class '" << className << "'";

    rapidjson::Value::ConstObject members = classes[className.c_str()].GetObject();

    if (!members.HasMember("methods"))
        ADD_FAILURE() << "Unable to find 'methods' member in class '" << className << "'";

    return members["methods"].GetArray();
}

void Cpp2JsonVisitorFixture::parseCpp(const std::string &path, bool const logJson)
{
    static llvm::cl::OptionCategory cpp2JsonCategory("Main options");
    std::vector<char const*> argv{"", path.c_str(), "--", CPP2JSON_SHARED_HEADER, "-std=c++14"};
    int argc = static_cast<int>(argv.size());

    clang::tooling::CommonOptionsParser optionParser(argc, argv.data(), cpp2JsonCategory);
    clang::tooling::ClangTool tool(optionParser.getCompilations(), optionParser.getSourcePathList());
    clang::tooling::CommandLineArguments extraArguments = makeLlvmIncludeArguments();
    auto extra = clang::tooling::getInsertArgumentAdjuster(extraArguments, clang::tooling::ArgumentInsertPosition::END);
    auto const parameters = makeParameters();

    tool.appendArgumentsAdjuster(extra);

    auto const frontEndActionFactory = newCpp2JsonActionFactory(parameters, m_document);
    auto const result = tool.run(frontEndActionFactory.get());

    ASSERT_EQ (result, 0);

    if (result == 0 && logJson)
    {
        JsonOutputWriter const writer(parameters);

        writer.write(m_document);
    }
}

bool testFieldInfo(const rapidjson::Value::ConstArray &fields, const std::string &fieldName, const std::string &fieldQualifier, const bool expected, bool &fieldFound)
{
    bool result = false;

    fieldFound = false;
    for (auto it = fields.Begin(); it != fields.End(); ++it)
    {
        auto const field = it->GetObject();

        if (field["name"].GetString() == fieldName)
        {
            auto const qualifiers = field["type"].GetObject();

            result = (qualifiers[fieldQualifier.c_str()].GetBool() == expected);
            fieldFound = true;
            break;
        }
    }
    return result;
}

testing::AssertionResult assertFieldHaveQualifier(const char *fieldsExpression, const char *fieldNameExpression,
                                                  const char *fieldQualifierExpression, const char *expectQualifierExpression,
                                                  const rapidjson::Value::ConstArray &fields, const std::string &fieldName,
                                                  const std::string &fieldQualifier, const bool expected)
{
    bool fieldFound = false;

    if (testFieldInfo(fields, fieldName, fieldQualifier, expected, fieldFound))
    {
        return ::testing::AssertionSuccess();
    }

    if (!fieldFound)
    {
        return ::testing::AssertionFailure() << "'" << fieldName << "' not found";
    }
    if (expected)
    {
        return ::testing::AssertionFailure() << "Field '" << fieldName << "' is not " << fieldQualifier;
    }
    else
    {
        return ::testing::AssertionFailure() << "Field '" << fieldName << "' is " << fieldQualifier;
    }
}