#include <gtest/gtest.h>
#include "Cpp2JsonVisitorFixture.hpp"
#include <Cpp2Json.hpp>

TEST_F(Cpp2JsonVisitorFixture, SimpleFieldsSimpleQualifiers)
{
    parseCpp(ResourcesPath + "/test_type_qualifier.hpp");

    rapidjson::Value::ConstArray fields = getFieldsOf("SimpleFieldsSimpleQualifiers");

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

TEST_F(Cpp2JsonVisitorFixture, PointerReferenceConstantQualifiers)
{
    parseCpp(ResourcesPath + "/test_type_qualifier.hpp");

    rapidjson::Value::ConstArray fields = getFieldsOf("PointerReferenceConstantQualifiers");

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer_const", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer_const", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer_const", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer_const", "pointer", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer_const", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer_const", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_pointer_const", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_const_pointer", "const", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_const_pointer", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_const_pointer", "array", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_const_pointer", "pointer", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_const_pointer", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_const_pointer", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "raw_const_pointer", "enum", false);
}

TEST_F(Cpp2JsonVisitorFixture, ConstantOneDimensionalArray)
{
    parseCpp(ResourcesPath + "/test_type_qualifier.hpp");

    rapidjson::Value::ConstArray fields = getFieldsOf("ConstantArray");

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_raw", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_raw", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_raw", "array", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_raw", "pointer", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_raw", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_raw", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_raw", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer", "array", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer", "pointer", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_const_pointer", "const", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_const_pointer", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_const_pointer", "array", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_const_pointer", "pointer", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_const_pointer", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_const_pointer", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_const_pointer", "enum", false);

    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer_const", "const", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer_const", "volatile", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer_const", "array", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer_const", "pointer", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer_const", "reference", false);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer_const", "literal", true);
    EXPECT_PRED_FORMAT4(assertFieldHaveQualifier, fields, "array_pointer_const", "enum", false);
}

TEST_F(Cpp2JsonVisitorFixture, FieldAndMethodExclusion)
{
    parseCpp(ResourcesPath + "/test_type_qualifier.hpp");

    rapidjson::Value::ConstArray fields = getFieldsOf("FieldAndMethodExclusion");
    rapidjson::Value::ConstArray methods = getMethodsOf("FieldAndMethodExclusion");

    EXPECT_EQ( fields.Size(), 0 );
    EXPECT_EQ( methods.Size(), 0 );
}
