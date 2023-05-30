#include "pch.h"
#include "InfoViewModel.h"
#if __has_include("InfoViewModel.g.cpp")
#include "InfoViewModel.g.cpp"
#endif

namespace winrt::Player::implementation
{
    InfoViewModel::InfoViewModel(winrt::Data::Music const& info)
    {
        info_ = info;
    }

    winrt::Data::Music InfoViewModel::Get()
    {
        return info_;
    }

    winrt::hstring InfoViewModel::Title()
    {
        return info_.Title;
    }

    winrt::hstring InfoViewModel::Album()
    {
        return info_.Album;
    }

    winrt::hstring InfoViewModel::Genre()
    {
        return info_.Genre;
    }

    winrt::hstring InfoViewModel::Artist()
    {
        return info_.Artist;
    }

    winrt::hstring InfoViewModel::Albumartist()
    {
        return info_.Albumartist;
    }

    winrt::hstring InfoViewModel::Path()
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

    winrt::hstring InfoViewModel::TransformBitrate(uint32_t value)
    {
        return fast_io::wconcat_winrt_hstring(value / 1000, L"kbps");
    }

    winrt::hstring InfoViewModel::TransformDuration(int64_t value)
    {
        auto const hms{ std::chrono::hh_mm_ss{ winrt::clock::duration{ static_cast<int64_t>(value) } } };
        auto hours{ hms.hours().count() };
        if (hours)
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hours, 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
        else
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
    }

    winrt::hstring InfoViewModel::DecisionTitle(winrt::hstring const& title, winrt::hstring const& path)
    {
        if (title.empty()) [[unlikely]]
        {
            auto path_sv{ std::wstring_view{ path } };
            auto end{ path_sv.find_last_of(L'.') };
            auto begin{ path_sv.find_last_of(L'\\') };
            return winrt::hstring{ path_sv.substr(begin + 1uz, end - 1uz) };
        }
        return title;
    }

    const winrt::hstring& InfoViewModel::DecisionArtist(winrt::hstring const& artist, winrt::hstring const& albumartist)
    {
        if (artist.empty()) [[unlikely]]
            return albumartist;
        return artist;
    }
}
