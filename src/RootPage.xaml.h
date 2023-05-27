#pragma once

#include "RootPage.g.h"
#include "SettingsHelper.h"

namespace winrt::Player::implementation
{
    struct RootPage : RootPageT<RootPage>
    {
        RootPage();
        // rumtime properties
        winrt::hstring AppTitleText();
        winrt::Player::PlayerViewModel PlayerViewModel();

        // events
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Volume_LostFocus(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Page_ActualThemeChanged(winrt::Microsoft::UI::Xaml::FrameworkElement const&, winrt::Windows::Foundation::IInspectable const&);
        void Mute_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Next_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Repeat_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Folders_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Shuffle_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Current_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Previous_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void PlayButton_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Navigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const&, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const&);
        Windows::Foundation::IAsyncAction Add_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        Windows::Foundation::IAsyncAction About_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        Windows::Foundation::IAsyncAction Equalizer_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        Windows::Foundation::IAsyncAction MusicInfo_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Navigation_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const&, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&);

    private:
        // UI helpers
        winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem MakeNavItem(const winrt::Data::Library&);
        void PlayButtonOn();
        void PlayButtonOff();
        void NavigateToDefaultPage();
        void InitializeRegistEvents();

        bool repeat_one_{ false };
        winrt::Data::Library library_{};
        winrt::hstring title_{L"PlayerWinRT"};
        Windows::Media::Playback::MediaPlayer player_{};
        winrt::Player::PlayerViewModel playerViewModel_{};
        Windows::Media::Playback::MediaPlaybackList list_{};
        winrt::Windows::Media::Playback::MediaPlaybackSession session_{ player_.PlaybackSession() };
        winrt::Windows::Media::Playback::MediaPlaybackCommandManager commander_{ player_.CommandManager()};
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> libraries_{ SettingsHelper::GetLibraries() };
        winrt::Windows::Foundation::Collections::IObservableVector<Windows::Media::Playback::MediaPlaybackItem> music_{ list_.Items() };
        winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> info_list_{ winrt::single_threaded_vector<winrt::Data::Music>() };
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
