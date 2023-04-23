#pragma once

#include "PlayList.g.h"

namespace winrt::Player::implementation
{
    struct PlayList : PlayListT<PlayList>
    {
        PlayList();
    };
}

namespace winrt::Player::factory_implementation
{
    struct PlayList : PlayListT<PlayList, implementation::PlayList>
    {
    };
}
