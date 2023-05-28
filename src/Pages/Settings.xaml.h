#pragma once

#include "Settings.g.h"

namespace winrt::Player::implementation
{
    struct Settings : SettingsT<Settings>
    {
        Settings();
        static winrt::hstring GetLangTagName(winrt::hstring const&);
        void Theme_Loaded(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Theme_Changed(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        winrt::Windows::Foundation::IAsyncAction Reset_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    private:
        static void Language_Selected(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Settings : SettingsT<Settings, implementation::Settings>
    {
    };
}
