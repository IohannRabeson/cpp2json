#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/process.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>

static boost::filesystem::path const ExecutablePath = EXECUTABLE_PATH;
static boost::filesystem::path const TestDataDir = TEST_DATA_PATH;
static boost::filesystem::path const InputDataDir = TestDataDir / "datas";
static boost::filesystem::path const ResultDataDir = TestDataDir / "results";

static std::string const CppInputs[] =
{
    "test_integral_types.hpp",
    "test_stl_containers.hpp",
    "test_integral_array_types.hpp"
};

static std::string const JsonResults[] =
{
    "test_integral_types.json",
    "test_stl_containers.json",
    "test_integral_array_types.json"
};

rapidjson::Document readDocument(std::istream& is)
{
    rapidjson::Document document;
    rapidjson::IStreamWrapper isw(is);

    document.ParseStream(isw);
    BOOST_REQUIRE(document.HasParseError() == false);
    return document;
}

rapidjson::Document readDocument(boost::filesystem::path const& path)
{
    std::cout << "Loading document from file '" << path << std::endl;
    boost::filesystem::ifstream file(path);

    return readDocument(file);
}

std::string documentToString(rapidjson::Document const& document)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    document.Accept(writer);
    return buffer.GetString();
}

BOOST_AUTO_TEST_SUITE(test_cpp2json)

BOOST_DATA_TEST_CASE(test_cpp2json_test01, boost::unit_test::data::make(CppInputs) ^ JsonResults,
                                           inputPath,
                                           resultPath)
{
    boost::filesystem::path const inputFilePath = InputDataDir / inputPath;
    boost::filesystem::path const resultFilePath = ResultDataDir / resultPath;

    boost::process::ipstream ips;
    boost::process::child process(ExecutablePath, "--output=-", inputFilePath, "--", boost::process::std_out > ips);

    process.join();

    auto const expectedResults = readDocument(resultFilePath);
    auto const realResults = readDocument(ips);

    BOOST_TEST((expectedResults == realResults));
}

BOOST_AUTO_TEST_SUITE_END()
