#pragma once

#include "Welcome.g.h"

namespace winrt::Player::implementation
{
    struct Welcome : WelcomeT<Welcome>
    {
        Welcome();
        Windows::Foundation::IAsyncAction MusicFolder_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        Windows::Foundation::IAsyncAction OtherFolder_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Theme_Click(IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Welcome : WelcomeT<Welcome, implementation::Welcome>
    {
    };
}
