#pragma once

#include <string>

namespace services::train
{

    enum class CardMode
    {
        SOURCE_TO_TARGET_INPUT = 1,
        TARGET_TO_SOURCE_INPUT = 2,
        SOURCE_TO_TARGET_SELECT = 3,
        TARGET_TO_SOURCE_SELECT = 4,
    };

    inline std::string toString(CardMode mode)
    {
        switch (mode)
        {
        case CardMode::SOURCE_TO_TARGET_INPUT:
            return "source_to_target_input";
        case CardMode::TARGET_TO_SOURCE_INPUT:
            return "target_to_source_input";
        case CardMode::SOURCE_TO_TARGET_SELECT:
            return "source_to_target_select";
        case CardMode::TARGET_TO_SOURCE_SELECT:
            return "target_to_source_select";
        default:
            return "UNKNOWN";
        }
    }

    inline CardMode fromString(const std::string &str)
    {
        if (str == "source_to_target_input")
        {
            return CardMode::SOURCE_TO_TARGET_INPUT;
        }
        else if (str == "target_to_source_input")
        {
            return CardMode::TARGET_TO_SOURCE_INPUT;
        }
        else if (str == "source_to_target_select")
        {
            return CardMode::SOURCE_TO_TARGET_SELECT;
        }
        else if (str == "target_to_source_select")
        {
            return CardMode::TARGET_TO_SOURCE_SELECT;
        }
        else
        {
            throw std::invalid_argument("Invalid CardMode string: " + str);
        }
    }

}
