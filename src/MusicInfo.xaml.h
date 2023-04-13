// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MusicInfo.g.h"

namespace winrt::Player::implementation
{
    struct MusicInfo : MusicInfoT<MusicInfo>
    {
        MusicInfo();
    };
}

namespace winrt::Player::factory_implementation
{
    struct MusicInfo : MusicInfoT<MusicInfo, implementation::MusicInfo>
    {
    };
}
