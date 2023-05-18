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
    private:
        std::vector<winrt::Data::MusicInfo> library_;
        std::vector<std::vector<std::reference_wrapper<const winrt::Data::MusicInfo>>> folders_stack_;
        std::vector<winrt::hstring> path_stack_;
        // immediately initialized for binding
        void BuildRoot();
        void Build();
        void Rebuild();
        void UpdateCountedUI(std::size_t, std::size_t);
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
