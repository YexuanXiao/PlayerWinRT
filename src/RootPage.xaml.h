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
        void AppTitleText(winrt::hstring const&);
        Windows::Foundation::IAsyncAction Navigation_ItemInvoked(Microsoft::UI::Xaml::Controls::NavigationView const&, Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const&);
        void Navigation_Loaded(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        Microsoft::UI::Xaml::Controls::Frame GetRootFrame();
        Microsoft::UI::Xaml::Controls::Grid GetAppTitleBar();
        Windows::Foundation::IAsyncAction MusicInfo_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void On_Loaded(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void PlayButton_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Repeat_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void Shuffle_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> Libraries();
        static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::MusicInfo> Music();
        winrt::Player::PlayerViewModel PlayerMainViewModel();
    private:
        void UpdateVolume(IInspectable const&, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const&);
        static winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem MakeNavItem(const winrt::Data::Library&);
        inline static winrt::Player::PlayerViewModel playerViewModel_;
        Windows::Media::Playback::MediaPlayer player_;
        Windows::Media::Playback::MediaPlaybackList list_;
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
