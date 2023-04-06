// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "About.g.h"

namespace winrt::Player::implementation
{
    struct About : AboutT<About>
    {
        About();
    };
}

namespace winrt::Player::factory_implementation
{
    struct About : AboutT<About, implementation::About>
    {
    };
}
