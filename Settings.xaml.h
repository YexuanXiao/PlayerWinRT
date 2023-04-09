// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "Settings.g.h"

namespace winrt::Player::implementation
{
    struct Settings : SettingsT<Settings>
    {
        Settings();
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Settings : SettingsT<Settings, implementation::Settings>
    {
    };
}
