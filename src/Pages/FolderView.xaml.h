#pragma once

#include "FolderView.g.h"

namespace winrt::Player::implementation
{
    struct FolderView: FolderViewT<FolderView>
    {
        FolderView();

        // binding helper
        winrt::hstring CalculateTrueFolderCount(std::size_t);
        // event handlers
        winrt::Windows::Foundation::IAsyncAction OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);

    private:
        // data for private use
        std::vector<winrt::hstring> path_stack_;
        std::vector<winrt::Data::Music> whole_library;
        std::vector<std::vector<std::reference_wrapper<const winrt::Data::Music>>> folders_stack_;

        // data from root page
        winrt::Data::Library library_{};
        winrt::Player::PlayerViewModel player_view_model_{ nullptr };
        winrt::Windows::Media::Playback::MediaPlaybackList play_list_{ nullptr };
        winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> info_list_{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Media::Playback::MediaPlaybackItem> music_list_{ nullptr };

        // UI helpers
        void Build();
        void Rebuild();
        void BuildRoot();
        void UpdateUI(std::vector<winrt::hstring>&&, std::vector<winrt::Player::InfoViewModel>&&);

        // runtime properties implement
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> folders_view_{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> music_view_{ nullptr };

        // events
        winrt::event_token sync_fvl_{};
        winrt::event_token sync_mvl_click_{};
        winrt::event_token sync_mvl_select_{};
        winrt::event_token sync_pl_{};
    };
}

namespace winrt::Player::factory_implementation
{
    struct FolderView: FolderViewT<FolderView, implementation::FolderView>
    {
    };
}
