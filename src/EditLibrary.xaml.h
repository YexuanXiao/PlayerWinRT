#pragma once

#include "EditLibrary.g.h"

namespace winrt::Player::implementation
{

    struct EditLibrary : EditLibraryT<EditLibrary>
    {
        EditLibrary();
        EditLibrary(winrt::hstring const&, winrt::hstring const&, winrt::hstring const&, winrt::hstring const&);
        void ProtocolMenu_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        Windows::Foundation::IAsyncAction SelectButton_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        winrt::Data::Library GetResult();
    private:
        void Icon_Select(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct EditLibrary : EditLibraryT<EditLibrary, implementation::EditLibrary>
    {
    };
}
