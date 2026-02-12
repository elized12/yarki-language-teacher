#include <boost/locale.hpp>
#include <boost/locale/boundary.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <algorithm>
#include <string>

namespace utils::string
{

    inline std::string &ltrim(std::string &s)
    {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), [](char symbol)
                             { return !std::isspace(symbol); }));

        return s;
    }

    inline std::string &rtrim(std::string &s)
    {
        s.erase(std::find_if(
                    s.rbegin(), s.rend(), [](char symbol)
                    { return !std::isspace(symbol); })
                    .base(),
                s.end());

        return s;
    }

    inline void trim(std::string &s)
    {
        ltrim(s);
        rtrim(s);
    }

    inline void trimUtf8(std::string &s)
    {
        boost::algorithm::trim(s);
    }

    inline bool isValidUuid(const std::string &uuid)
    {
        try
        {
            boost::uuids::string_generator gen;
            gen(uuid);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

} // namespace utils::string