#pragma once

#include "RootPage.g.h"

namespace winrt::Player::implementation
{
    struct RootPage : RootPageT<RootPage>
    {
        RootPage();
        Windows::Media::Playback::MediaPlayer Player();
        Windows::Media::Playback::MediaPlaybackList List();
        winrt::hstring AppTitleText();
        winrt::Player::PlayerViewModel PlayerMainViewModel();
        static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> Libraries();
        static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::MusicInfo> Music();
        // events
        Windows::Foundation::IAsyncAction Navigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const&, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const&);
        Windows::Foundation::IAsyncAction MusicInfo_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Repeat_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Shuffle_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Current_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Navigation_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const&, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&);
    private:
        winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem MakeNavItem(const winrt::Data::Library&);
        bool TogglePlayButton();
        void NavigateToDefaultPage();
        winrt::hstring title_{L"PlayerWinRT"};
        Windows::Media::Playback::MediaPlayer player_;
        winrt::Windows::Media::Playback::MediaPlaybackSession session_;
        winrt::Windows::Media::Playback::MediaPlaybackCommandManager commander_;
        inline static Windows::Media::Playback::MediaPlaybackList list_{};
        inline static winrt::Player::PlayerViewModel playerViewModel_{};
        inline static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> libraries_{nullptr};
        inline static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::MusicInfo> music_{nullptr};
    };
}

namespace winrt::Player::factory_implementation
{
    struct RootPage : RootPageT<RootPage, implementation::RootPage>
    {
    };
}

/// <summary>
///  RootPage::AppTitleBar has two functions: 1. To be compatible with Windows 10,
/// it needs to get the title bar element for Window::SetTitleBar;
/// 2. It calculates the draggable area through the AppTitleBar element itself.
/// If no need to compable with Windows 10, this function can be changed to GetDragRectangles,
/// directly obtains the draggable area.
/// </summary>
