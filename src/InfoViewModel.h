#pragma once
#include "InfoViewModel.g.h"

namespace winrt::Player::implementation
{
    struct InfoViewModel : InfoViewModelT<InfoViewModel>
    {
        InfoViewModel() = default;

        InfoViewModel(winrt::Data::MusicInfo const& info);
        winrt::Data::MusicInfo Get();
        hstring Title();
        hstring Album();
        hstring Genre();
        hstring Artist();
        hstring Albumartist();
        hstring Path();
        int64_t Duration();
        uint32_t Year();
        uint32_t Bitrate();
        uint32_t Track();

    private:
        winrt::Data::MusicInfo info_;
    };
}
namespace winrt::Player::factory_implementation
{
    struct InfoViewModel : InfoViewModelT<InfoViewModel, implementation::InfoViewModel>
    {
    };
}