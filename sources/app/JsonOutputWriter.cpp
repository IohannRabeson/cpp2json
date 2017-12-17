#include "JsonOutputWriter.hpp"
#include "Cpp2JsonParameters.hpp"

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include <fstream>
#include <iostream>

JsonOutputWriter::JsonOutputWriter(Cpp2JsonParameters const& parameters) :
    m_parameters(parameters)
{
}

bool JsonOutputWriter::write(rapidjson::Document const& document) const
{
    bool result = false;

    if (m_parameters.outputFilePath == "-")
    {
        writeJsonToOStream(document, std::cout);
        result = true;
    }
    else
    {
        std::ofstream file(m_parameters.outputFilePath, std::ios_base::out | std::ios_base::ate);

        if (file.is_open())
        {
            result = writeJsonToOStream(document, file);
        }
        else
        {
            std::cerr << "Unable to open '" << m_parameters.outputFilePath << "' for writing." << std::endl;
        }
    }
    return result;
}

bool JsonOutputWriter::writeJsonToOStream(rapidjson::Document const& document, std::ostream& os) const
{
    rapidjson::OStreamWrapper streamWrapper{os};

    if (m_parameters.prettyOutput)
    {
        rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{streamWrapper};

        return document.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::OStreamWrapper> writer{streamWrapper};

        return document.Accept(writer);
    }
}
