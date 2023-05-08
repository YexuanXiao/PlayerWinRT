#pragma once

#include "EditLibrary.g.h"

namespace winrt::Player::implementation
{

    struct EditLibrary : EditLibraryT<EditLibrary>
    {
        EditLibrary();
        EditLibrary(winrt::hstring const&, winrt::hstring const&, winrt::hstring const&, winrt::hstring const&);
        void ProtocolMenu_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        Windows::Foundation::IAsyncAction SelectButton_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Icon_Select(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        winrt::Data::Library GetResult();
    };
}

namespace winrt::Player::factory_implementation
{
    struct EditLibrary : EditLibraryT<EditLibrary, implementation::EditLibrary>
    {
    };
}
