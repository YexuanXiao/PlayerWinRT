#pragma once

#include "RootPage.g.h"

namespace winrt::Player::implementation
{
    struct RootPage : RootPageT<RootPage>
    {
        RootPage();
        Windows::Media::Playback::MediaPlayer player_;
        Windows::Media::Playback::MediaPlaybackList list_;
        Windows::Media::Playback::MediaPlayer Player();
        Windows::Media::Playback::MediaPlaybackList List();
        winrt::Player::PlayerViewModel PlayerMainViewModel();
        winrt::hstring AppTitleText();
        void AppTitleText(winrt::hstring const&);
        Windows::Foundation::IAsyncAction Navigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const&, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const&);
        void Navigation_Loaded(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        Microsoft::UI::Xaml::Controls::Frame GetRootFrame();
        Microsoft::UI::Xaml::Controls::Grid GetAppTitleBar();
        Windows::Foundation::IAsyncAction MusicInfo_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void On_Loaded(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void PlayButton_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Repeat_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Shuffle_Click(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void UpdateVolume(IInspectable const&, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const&);
    private:
        winrt::Player::PlayerViewModel playerViewModel_;
    };
}

namespace winrt::Player::factory_implementation
{
    struct RootPage : RootPageT<RootPage, implementation::RootPage>
    {
    };
}
