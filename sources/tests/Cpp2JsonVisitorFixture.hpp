#ifndef CPP2JSONVISITORFIXTURE_HPP
#define CPP2JSONVISITORFIXTURE_HPP
#include <gtest/gtest.h>
#include <rapidjson/document.h>

static std::string const ResourcesPath{RESOURCE_DIR};

class Cpp2JsonVisitorFixture : public ::testing::Test
{
protected:
    Cpp2JsonVisitorFixture();

    void parseCpp(std::string const& path);
    rapidjson::Value::ConstObject getEnums() const;
    rapidjson::Value::ConstObject getClasses() const;
    rapidjson::Value::ConstArray getFieldsOf(std::string const& className) const;
private:
    rapidjson::Document m_document;
};

#endif // CPP2JSONVISITORFIXTURE_HPP


