﻿#include "pch.h"
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

    winrt::Data::Music InfoViewModel::Get() const
    {
        return info_;
    }

    winrt::hstring InfoViewModel::Title() const
    {
        return info_.Title;
    }

    winrt::hstring InfoViewModel::Album() const
    {
        return info_.Album;
    }

    winrt::hstring InfoViewModel::Genre() const
    {
        return info_.Genre;
    }

    winrt::hstring InfoViewModel::Artist() const
    {
        return info_.Artist;
    }

    winrt::hstring InfoViewModel::Albumartist() const
    {
        return info_.Albumartist;
    }

    winrt::hstring InfoViewModel::Path() const
    {
        return info_.Path;
    }

    int64_t InfoViewModel::Duration() const
    {
        return info_.Duration;
    }

    uint32_t InfoViewModel::Year() const
    {
        return info_.Year;
    }

    uint32_t InfoViewModel::Bitrate() const
    {
        return info_.Bitrate;
    }

    uint32_t InfoViewModel::Track() const
    {
        return info_.Track;
    }

    winrt::hstring InfoViewModel::State() const
    {
        return is_active_ ? NoiseCancelation : EmptyString;
    }

    void InfoViewModel::SetState(bool value)
    {
        if (value == is_active_)
            return;

        is_active_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"State" });
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

    winrt::event_token InfoViewModel::PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChanged_.add(handler);
    }

    void InfoViewModel::PropertyChanged(winrt::event_token const& token) noexcept
    {
        propertyChanged_.remove(token);
    }
}
