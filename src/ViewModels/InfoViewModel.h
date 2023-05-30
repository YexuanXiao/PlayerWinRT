#pragma once

#include "InfoViewModel.g.h"

namespace winrt::Player::implementation
{
    struct InfoViewModel: InfoViewModelT<InfoViewModel>
    {
        InfoViewModel() = default;
        InfoViewModel(winrt::Data::Music const& info);

        winrt::hstring Title();
        winrt::hstring Album();
        winrt::hstring Genre();
        winrt::hstring Artist();
        winrt::hstring Albumartist();
        winrt::hstring Path();
        int64_t Duration();
        uint32_t Year();
        uint32_t Bitrate();
        uint32_t Track();

        static winrt::hstring TransformBitrate(uint32_t);
        static winrt::hstring TransformDuration(int64_t);
        static winrt::hstring DecisionTitle(winrt::hstring const&, winrt::hstring const&);
        static const winrt::hstring& DecisionArtist(winrt::hstring const&, winrt::hstring const&);

        winrt::Data::Music Get();

    private:
        winrt::Data::Music info_;
    };
}

namespace winrt::Player::factory_implementation
{
    struct InfoViewModel: InfoViewModelT<InfoViewModel, implementation::InfoViewModel>
    {
    };
}
