#pragma once

#include "Settings.g.h"

namespace winrt::Player::implementation
{
    struct Settings: SettingsT<Settings>
    {
        Settings();
        void Theme_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Theme_Changed(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        winrt::Windows::Foundation::IAsyncAction Reset_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);

    private:
        static void Language_Selected(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Settings: SettingsT<Settings, implementation::Settings>
    {
    };
}
