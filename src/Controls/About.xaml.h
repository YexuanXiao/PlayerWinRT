#pragma once

#include "About.g.h"

namespace winrt::Player::implementation
{
    struct About: AboutT<About>
    {
        About();
    };
}

namespace winrt::Player::factory_implementation
{
    struct About: AboutT<About, implementation::About>
    {
    };
}
