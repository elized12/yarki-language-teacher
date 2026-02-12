#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <ctime>
#include <iomanip>

namespace models
{

    using id = std::uint64_t;
    using timePoint = std::chrono::system_clock::time_point;

    inline std::string timePointToString(const std::chrono::system_clock::time_point &tp)
    {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    inline std::chrono::system_clock::time_point stringToTimePoint(const std::string &str)
    {
        std::tm tm{};
        std::istringstream iss(str);
        iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (iss.fail())
        {
            throw std::runtime_error("Не удалось преобразовать строку в time_point: " + str);
        }
        std::time_t t = std::mktime(&tm);
        return std::chrono::system_clock::from_time_t(t);
    }

} // namespace models