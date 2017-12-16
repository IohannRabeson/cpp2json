#include <gtest/gtest.h>
#include "Cpp2JsonVisitorFixture.hpp"

bool testFieldInfo(rapidjson::Value::ConstArray const& fields, std::string const& fieldName,
                   std::string const& fieldQualifier, bool const expected,
                   bool& fieldFound)
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

::testing::AssertionResult assertFieldHaveQualifier(char const* fieldsExpression,
                                                    char const* fieldNameExpression,
                                                    char const* fieldQualifierExpression,
                                                    char const* expectQualifierExpression,
                                                    rapidjson::Value::ConstArray const& fields,
                                                    std::string const& fieldName,
                                                    std::string const& fieldQualifier,
                                                    bool const expected)
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

TEST_F(Cpp2JsonVisitorFixture, SimpleFieldsSimpleQualifiers)
{
    parseCpp(ResourcesPath + "/test_type_qualifier.hpp");

    rapidjson::Value::ConstArray fields = getFieldsOf("TestTypeQualifier");

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw", "pointer", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer", "pointer", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "reference", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "reference", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "reference", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "reference", "pointer", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "reference", "reference", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "reference", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "reference", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw", "const", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw", "pointer", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw_pointer", "const", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw_pointer", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw_pointer", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw_pointer", "pointer", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw_pointer", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw_pointer", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_raw_pointer", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_reference", "const", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_reference", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_reference", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_reference", "pointer", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_reference", "reference", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_reference", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_reference", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "volatile_raw", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "volatile_raw", "volatile", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "volatile_raw", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "volatile_raw", "pointer", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "volatile_raw", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "volatile_raw", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "volatile_raw", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_volatile_raw", "const", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_volatile_raw", "volatile", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_volatile_raw", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_volatile_raw", "pointer", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_volatile_raw", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_volatile_raw", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "const_volatile_raw", "enum", false);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
