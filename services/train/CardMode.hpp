#pragma once

namespace services::train
{

    enum class CardMode
    {
        SOURCE_TO_TARGET_INPUT = 1,
        TARGET_TO_SOURCE_INPUT = 2,
        SOURCE_TO_TARGET_SELECT = 3,
        TARGET_TO_SOURCE_SELECT = 4,
    };

}
