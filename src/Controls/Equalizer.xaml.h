#pragma once

#include "Equalizer.g.h"

namespace winrt::Player::implementation
{
    struct Equalizer: EqualizerT<Equalizer>
    {
        Equalizer();
    };
}

namespace winrt::Player::factory_implementation
{
    struct Equalizer: EqualizerT<Equalizer, implementation::Equalizer>
    {
    };
}
