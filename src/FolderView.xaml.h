#pragma once

#include "FolderView.g.h"

namespace winrt::Player::implementation
{
    struct FolderView: FolderViewT<FolderView>
    {
        FolderView();
        winrt::Windows::Foundation::IAsyncAction FolderView_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FolderList();
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> MusicList();
        static winrt::hstring TransformBitrate(uint32_t);
        static winrt::hstring TransformDuration(int64_t);
        static winrt::hstring DecisionTitle(winrt::hstring const&, winrt::hstring const&);
        static const winrt::hstring& DecisionArtist(winrt::hstring const&, winrt::hstring const&);
        winrt::hstring CalculateTrueFolderCount(uint32_t);
        winrt::Windows::Foundation::IAsyncAction OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);
    private:
        std::vector<winrt::Data::MusicInfo> library_;
        std::vector<std::vector<std::reference_wrapper<const winrt::Data::MusicInfo>>> folders_stack_;
        std::vector<winrt::hstring> path_stack_;
        // immediately initialized for binding
        void BuildRoot();
        winrt::Data::Library library_info_;
        winrt::Windows::Foundation::IAsyncAction Initialize();
        void Build();
        void Rebuild();
        void UpdateUI(std::vector<winrt::hstring> const&, std::vector<winrt::Player::InfoViewModel> const&);
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> folders_view_{ winrt::single_threaded_observable_vector<winrt::hstring>()};
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> music_view_{winrt::single_threaded_observable_vector<winrt::Player::InfoViewModel>()};
    };
}

namespace winrt::Player::factory_implementation
{
    struct FolderView: FolderViewT<FolderView, implementation::FolderView>
    {
    };
}
