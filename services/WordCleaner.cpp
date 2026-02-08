
#include "services/WordCleaner.hpp"

using namespace services;

const icu::UnicodeString WordCleaner::CLEAN_OTHER_PATTERN = icu::UnicodeString("[^\\p{L}\\s]");

const icu::UnicodeString WordCleaner::CLEAN_SPACE = icu::UnicodeString("\\s{2,}");

std::string WordCleaner::clean(const std::string &str)
{
    icu::UnicodeString utfString = icu::UnicodeString::fromUTF8(str);
    utfString.toLower();
    utfString.trim();

    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher matcherOther(this->CLEAN_OTHER_PATTERN, utfString, 0, status);
    utfString = matcherOther.replaceAll("", status);

    if (U_FAILURE(status))
    {
        throw WordCleanException("Error clean word");
    }

    icu::RegexMatcher matcherSpace(this->CLEAN_SPACE, utfString, 0, status);
    utfString = matcherSpace.replaceAll(icu::UnicodeString(" "), status);

    if (U_FAILURE(status))
    {
        throw WordCleanException("Error clean word");
    }

    std::string result;
    utfString.toUTF8String(result);

    return result;
}