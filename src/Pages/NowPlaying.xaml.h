#pragma once

#include "NowPlaying.g.h"

namespace winrt::Player::implementation
{
    struct NowPlaying: NowPlayingT<NowPlaying>
    {
        NowPlaying();
        ~NowPlaying();
        winrt::Player::PlayerViewModel PlayerViewModel();
        void OnNavigatingFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigatingCancelEventArgs const&);
        winrt::Windows::Foundation::IAsyncAction OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> MusicList();
        void ListView_ItemClick(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const&);
        void Remove_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);

    private:
        winrt::event_token sync_mv_{};
        winrt::event_token sync_pvm_{};
        winrt::event_token sync_pl_{};
        winrt::clock::rep count_;
        winrt::Data::Music removed_music_;
        winrt::Windows::Media::Playback::MediaPlaybackItem removed_item_{ nullptr };
        uint32_t index_;
        // data from rootpage
        winrt::Player::PlayerViewModel player_view_model_root_{ nullptr };
        winrt::Windows::Media::Playback::MediaPlaybackList play_list_{ nullptr };
        winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> info_list_{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Media::Playback::MediaPlaybackItem> music_list_{ nullptr };
        // runtime prop
        winrt::Player::PlayerViewModel player_view_model_{};
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> music_view_{ winrt::single_threaded_observable_vector<winrt::Player::InfoViewModel>() };
    };
}

namespace winrt::Player::factory_implementation
{
    struct NowPlaying: NowPlayingT<NowPlaying, implementation::NowPlaying>
    {
    };
}
