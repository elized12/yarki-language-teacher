#pragma once

#include <stdexcept>

namespace services
{

    class WordCleanException : public std::runtime_error
    {
    public:
        WordCleanException(const std::string &message) : std::runtime_error(message)
        {
        }
    };

} // namespace services