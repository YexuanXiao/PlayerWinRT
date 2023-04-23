#pragma once

#include "Welcome.g.h"

namespace winrt::Player::implementation
{
    struct Welcome : WelcomeT<Welcome>
    {
        Welcome();
    };
}

namespace winrt::Player::factory_implementation
{
    struct Welcome : WelcomeT<Welcome, implementation::Welcome>
    {
    };
}
