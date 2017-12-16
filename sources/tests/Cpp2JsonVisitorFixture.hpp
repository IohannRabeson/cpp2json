#ifndef CPP2JSONVISITORFIXTURE_HPP
#define CPP2JSONVISITORFIXTURE_HPP
#include <gtest/gtest.h>
#include <rapidjson/document.h>

class Cpp2JsonVisitorFixture : public ::testing::Test
{
    virtual void SetUp();
protected:
    void parseCpp(std::string const& path);
private:
    rapidjson::Document m_document;
};

#endif // CPP2JSONVISITORFIXTURE_HPP


