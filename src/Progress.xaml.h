// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "Progress.g.h"

namespace winrt::Player::implementation
{
    struct Progress : ProgressT<Progress>
    {
        Progress();
    };
}

namespace winrt::Player::factory_implementation
{
    struct Progress : ProgressT<Progress, implementation::Progress>
    {
    };
}
