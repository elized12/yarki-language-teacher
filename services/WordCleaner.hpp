#pragma once

#include <unicode/regex.h>
#include <unicode/unistr.h>

#include "services/WordCleanException.hpp"

namespace services
{
    class WordCleaner
    {
    private:
        static const icu::UnicodeString CLEAN_OTHER_PATTERN;
        static const icu::UnicodeString CLEAN_SPACE;

    public:
        std::string clean(const std::string &str);
    };

} // namespace services