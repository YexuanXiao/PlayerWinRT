#include "pch.h"
#include "InfoViewModel.h"
#if __has_include("InfoViewModel.g.cpp")
#include "InfoViewModel.g.cpp"
#endif

namespace winrt::Player::implementation
{
    InfoViewModel::InfoViewModel(winrt::Data::MusicInfo const& info)
    {
        info_ = info;
    }
    winrt::Data::MusicInfo InfoViewModel::Get() {
        return info_;
    }
    hstring InfoViewModel::Title()
    {
        return info_.Title;
    }
    hstring InfoViewModel::Album()
    {
        return info_.Album;
    }
    hstring InfoViewModel::Genre()
    {
        return info_.Genre;
    }
    hstring InfoViewModel::Artist()
    {
        return info_.Artist;
    }
    hstring InfoViewModel::Albumartist()
    {
        return info_.Albumartist;
    }
    hstring InfoViewModel::Path()
    {
        return info_.Path;
    }
    int64_t InfoViewModel::Duration()
    {
        return info_.Duration;
    }
    uint32_t InfoViewModel::Year()
    {
        return info_.Year;
    }
    uint32_t InfoViewModel::Bitrate()
    {
        return info_.Bitrate;
    }
    uint32_t InfoViewModel::Track()
    {
        return info_.Track;
    }
}
