#pragma once

#include "RootPage.g.h"
#include "winrt/Data.h"

namespace winrt::Player::implementation
{
    struct RootPage : RootPageT<RootPage>
    {
        RootPage();
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
        static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> Libraries();
    private:
        winrt::Player::PlayerViewModel playerViewModel_;
        Windows::Media::Playback::MediaPlayer player_;
        Windows::Media::Playback::MediaPlaybackList list_;
        inline static winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> libraries_{winrt::single_threaded_observable_vector<winrt::Data::Library>()};
    };
}

namespace winrt::Player::factory_implementation
{
    struct RootPage : RootPageT<RootPage, implementation::RootPage>
    {
    };
}
