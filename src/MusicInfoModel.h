#pragma once
#include "MusicInfoModel.g.h"

namespace winrt::Player::implementation
{
    struct MusicInfoModel : MusicInfoModelT<MusicInfoModel>
    {
        MusicInfoModel() = default;
        MusicInfoModel(hstring const&, hstring const&, hstring const&, hstring const&, hstring const&, hstring const&, int64_t, int32_t, int32_t, int32_t);
        hstring title_;
        hstring album_;
        hstring genre_;
        hstring artist_;
        hstring albumartist_;
        hstring path_;
        int64_t duration_;
        int32_t year_;
        uint32_t bitrate_;
        uint32_t track_;

        hstring title();
        void title(hstring const& value);
        hstring album();
        void album(hstring const& value);
        hstring genre();
        void genre(hstring const& value);
        hstring artist();
        void artist(hstring const& value);
        hstring albumartist();
        void albumartist(hstring const& value);
        hstring path();
        void path(hstring const& value);
        int64_t duration();
        void duration(int64_t value);
        int32_t year();
        void year(int32_t value);
        uint32_t bitrate();
        void bitrate(uint32_t value);
        uint32_t track();
        void track(uint32_t value);
    };
}
namespace winrt::Player::factory_implementation
{
    struct MusicInfoModel : MusicInfoModelT<MusicInfoModel, implementation::MusicInfoModel>
    {
    };
}
