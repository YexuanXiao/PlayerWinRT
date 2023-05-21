#pragma once

#include "RootPage.g.h"

namespace winrt::Player::implementation
{
    struct RootPage : RootPageT<RootPage>
    {
        RootPage();
        // rumtime properties
        winrt::hstring AppTitleText();
        Windows::Media::Playback::MediaPlayer Player();
        static winrt::Player::PlayerViewModel PlayerViewModel();
        static Windows::Media::Playback::MediaPlaybackList List();
        static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> Libraries();
        static winrt::Windows::Foundation::Collections::IObservableVector<Windows::Media::Playback::MediaPlaybackItem> Music();
        // events
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Volume_LostFocus(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Repeat_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Shuffle_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Current_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void PlayButton_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        Windows::Foundation::IAsyncAction MusicInfo_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Navigation_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const&, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&);
        Windows::Foundation::IAsyncAction Navigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const&, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const&);

    private:
        // UI helpers
        winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem MakeNavItem(const winrt::Data::Library&);
        void PlayButtonOn();
        void PlayButtonOff();
        void NavigateToDefaultPage();
        // regist events
        void InitializeRegistEvent();
        // rumtime properties implement
        winrt::hstring title_{L"PlayerWinRT"};
        Windows::Media::Playback::MediaPlayer player_;
        winrt::Windows::Media::Playback::MediaPlaybackSession session_;
        winrt::Windows::Media::Playback::MediaPlaybackCommandManager commander_;
        inline static winrt::Player::PlayerViewModel playerViewModel_;
        inline static Windows::Media::Playback::MediaPlaybackList list_{};
        inline static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> libraries_{ nullptr };
        inline static winrt::Windows::Foundation::Collections::IObservableVector<Windows::Media::Playback::MediaPlaybackItem> music_{list_.Items()};
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
