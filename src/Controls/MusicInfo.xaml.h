#pragma once

#include "MusicInfo.g.h"

namespace winrt::Player::implementation
{
    struct MusicInfo : MusicInfoT<MusicInfo>
    {
        MusicInfo(winrt::Data::MusicInfoParameter const&);
        winrt::hstring MusicTitle();
        winrt::hstring Artist();
        winrt::hstring Album();
        winrt::hstring Albumartist();
        winrt::hstring Track();
        winrt::hstring Duration();
        winrt::hstring Genre();
        winrt::hstring Year();
        winrt::hstring Bitrate();
        winrt::hstring Format();
        winrt::hstring Path();
    private:
        winrt::Data::Library library_;
        winrt::Data::Music music_;
    public:
        void Folder_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct MusicInfo : MusicInfoT<MusicInfo, implementation::MusicInfo>
    {
    };
}
