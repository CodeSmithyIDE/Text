// SPDX-FileCopyrightText: 2000-2024 Xavier Leclercq
// SPDX-License-Identifier: BSL-1.0

#include "InterpolatedString.hpp"
#include "TextErrorCategory.hpp"

using namespace Ishiko;

InterpolatedString::MapCallbacks::MapCallbacks(const std::map<std::string, std::string>& map, bool allowMissing)
    : m_map(map), m_allowMissing(allowMissing)
{
}

void InterpolatedString::MapCallbacks::expand(boost::string_view variable, std::string& result, Error& error) const
{
    std::map<std::string, std::string>::const_iterator it = m_map.find(variable.to_string());
    if (it != m_map.end())
    {
        result.append(it->second);
    }
    else if (!m_allowMissing)
    {
        Fail(TextErrorCategory::Value::generic, error);
    }
}

InterpolatedString::InterpolatedString()
{
}

InterpolatedString::InterpolatedString(const char* str)
    : m_string(str)
{
}

InterpolatedString::InterpolatedString(std::string str)
    : m_string(str)
{
}

std::string InterpolatedString::expand(const Callbacks& callbacks) const
{
    Ishiko::Error error;
    std::string result = expand(callbacks, error);
    ThrowIf(error);
    return result;
}

std::string InterpolatedString::expand(const Callbacks& callbacks, Error& error) const
{
    std::string result;

    size_t lastAddedPos = 0;
    const char* startPattern = "${";
    const char* endPattern = "}";
        
    size_t beginPos = m_string.find(startPattern, lastAddedPos);
    while (beginPos != std::string::npos)
    {
        size_t endPos = m_string.find(endPattern, beginPos);
        if (endPos != std::string::npos)
        {
            result += m_string.substr(lastAddedPos, beginPos - lastAddedPos);
            std::string name = m_string.substr(beginPos + 2, endPos - beginPos - 2);
            callbacks.expand(name, result, error);
            if (error)
            {
                return result;
            }
            beginPos = lastAddedPos = endPos + 1;
        }
        else
        {
            break;
        }
        beginPos = m_string.find(startPattern, beginPos);
    }

    result += m_string.substr(lastAddedPos);

    return result;
}
