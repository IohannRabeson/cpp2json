#ifndef CPP2JSONVISITORFIXTURE_HPP
#define CPP2JSONVISITORFIXTURE_HPP
#include <gtest/gtest.h>
#include <rapidjson/document.h>

static std::string const ResourcesPath{RESOURCE_DIR};

class Cpp2JsonVisitorFixture : public ::testing::Test
{
protected:
    Cpp2JsonVisitorFixture();

    void parseCpp(std::string const& path, bool const logJson = false);
    rapidjson::Value::ConstObject getEnums() const;
    rapidjson::Value::ConstObject getClasses() const;
    rapidjson::Value::ConstObject getField(std::string const& className, std::string const& fieldName) const;
    rapidjson::Value::ConstArray getFieldsOf(std::string const& className) const;
    rapidjson::Value::ConstArray getMethodsOf(std::string const& className) const;
private:
    rapidjson::Document m_document;
};

bool testFieldInfo(rapidjson::Value::ConstArray const& fields, std::string const& fieldName,
                   std::string const& fieldQualifier, bool const expected,
                   bool& fieldFound);

::testing::AssertionResult assertFieldHaveQualifier(char const* fieldsExpression,
                                                    char const* fieldNameExpression,
                                                    char const* fieldQualifierExpression,
                                                    char const* expectQualifierExpression,
                                                    rapidjson::Value::ConstArray const& fields,
                                                    std::string const& fieldName,
                                                    std::string const& fieldQualifier,
                                                    bool const expected);


#endif // CPP2JSONVISITORFIXTURE_HPP


