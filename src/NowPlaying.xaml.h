// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "NowPlaying.g.h"

namespace winrt::Player::implementation
{
    struct NowPlaying : NowPlayingT<NowPlaying>
    {
        NowPlaying();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::Player::factory_implementation
{
    struct NowPlaying : NowPlayingT<NowPlaying, implementation::NowPlaying>
    {
    };
}
