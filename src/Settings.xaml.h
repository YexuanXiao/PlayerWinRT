#pragma once

#include "Settings.g.h"

namespace winrt::Player::implementation
{
    struct Settings : SettingsT<Settings>
    {
        Settings();
        void Theme_Changed(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        void Theme_Loaded(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        static void Language_Selected(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        static hstring GetLangTagName(hstring const& tag);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Settings : SettingsT<Settings, implementation::Settings>
    {
    };
}
