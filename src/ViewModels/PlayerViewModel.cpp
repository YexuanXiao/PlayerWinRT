#include "pch.h"
#include "PlayerViewModel.h"
#if __has_include("PlayerViewModel.g.cpp")
#include "PlayerViewModel.g.cpp"
#endif
namespace winrt::Player::implementation
{
    double PlayerViewModel::Volume() const
    {
        return volume_;
    }

    void PlayerViewModel::Volume(double value)
    {
        if (value == volume_)
            return;

        volume_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Volume" });
    }

    uint32_t PlayerViewModel::Index() const
    {
        return index_;
    }

    void PlayerViewModel::Index(uint32_t value)
    {
        if (value == index_)
            return;

        index_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Index" });
    }

    double PlayerViewModel::Position() const
    {
        return position_;
    }

    void PlayerViewModel::Position(double value)
    {
        if (value == position_)
            return;

        position_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Position" });
    }

    double PlayerViewModel::Duration() const
    {
        return duration_;
    }

    void PlayerViewModel::Duration(double value)
    {
        if (value == duration_)
            return;

        duration_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Duration" });
    }

    winrt::Data::Library PlayerViewModel::Library() const
    {
        return library_;
    }

    void PlayerViewModel::Library(winrt::Data::Library const& value)
    {
        if (value.name == library_.name)
            return;

        library_ = value;
        // propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Library" });
    }

    winrt::hstring PlayerViewModel::Title() const
    {
        return title_;
    }

    void PlayerViewModel::Title(winrt::hstring const& value)
    {
        if (value == title_)
            return;

        title_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Title" });
    }

    winrt::hstring PlayerViewModel::AppTitle() const
    {
        return app_title_;
    }

    void PlayerViewModel::AppTitle(winrt::hstring const& value)
    {
        if (value == app_title_)
            return;

        app_title_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"AppTitle" });
    }

    winrt::hstring PlayerViewModel::Album() const
    {
        return album_;
    }

    void PlayerViewModel::Album(winrt::hstring const& value)
    {
        if (value == album_)
            return;

        album_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Album" });
    }

    winrt::hstring PlayerViewModel::Artist() const
    {
        return artist_;
    }

    void PlayerViewModel::Artist(winrt::hstring const& value)
    {
        if (value == artist_)
            return;

        artist_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Artist" });
    }

    winrt::Microsoft::UI::Xaml::Media::ImageSource PlayerViewModel::Image() const
    {
        return image_;
    }

    void PlayerViewModel::Image(winrt::Microsoft::UI::Xaml::Media::ImageSource const& value)
    {
        image_ = value;
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Image" });
    }

    winrt::hstring PlayerViewModel::DoubleToString(double value)
    {
        return fast_io::wconcat_winrt_hstring(value);
    }

    winrt::hstring PlayerViewModel::DurationToString(double value)
    {
        auto const hms{ std::chrono::hh_mm_ss{ winrt::clock::duration{ static_cast<int64_t>(value) } } };
        auto hours{ hms.hours().count() };
        if (hours)
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hours, 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
        else
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
    }

    winrt::event_token PlayerViewModel::PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChanged_.add(handler);
    }

    void PlayerViewModel::PropertyChanged(winrt::event_token const& token) noexcept
    {
        propertyChanged_.remove(token);
    }
}
