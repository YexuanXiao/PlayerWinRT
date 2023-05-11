#pragma once

#include "Welcome.g.h"

namespace winrt::Player::implementation
{
    struct Welcome : WelcomeT<Welcome>
    {
        Welcome();
        Windows::Foundation::IAsyncAction AddLibrary_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Theme_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Welcome : WelcomeT<Welcome, implementation::Welcome>
    {
    };
}
