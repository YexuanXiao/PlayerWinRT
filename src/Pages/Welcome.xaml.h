#pragma once

#include "Welcome.g.h"

namespace winrt::Player::implementation
{
    struct Welcome: WelcomeT<Welcome>
    {
        Welcome();
        winrt::Windows::Foundation::IAsyncAction OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);
        void Theme_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        winrt::Windows::Foundation::IAsyncAction AddLibrary_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);

    private:
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> libraries_{ nullptr };
        static void Language_Selected(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Welcome: WelcomeT<Welcome, implementation::Welcome>
    {
    };
}
