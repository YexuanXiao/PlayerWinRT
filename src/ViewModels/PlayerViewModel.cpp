#include "pch.h"
#include "PlayerViewModel.h"
#include "PlayerViewModel.g.cpp"

#include "SettingsHelper.h"

namespace winrt::Player::implementation
{
    PlayerViewModel::PlayerViewModel() {
        volume_ = SettingsHelper::GetVolume();
        position_ = 0.;
        duration_ = 0.;
    }
    double PlayerViewModel::Volume()
    {
        return volume_;
    }
    uint32_t PlayerViewModel::Index() {
        return index_;
    }
    double PlayerViewModel::Position() {
        return position_;
    }
    void PlayerViewModel::Position(double value) {
        if (value != position_) {
            position_ = value;
            propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Position" });
        }
    }
    double PlayerViewModel::Duration() {
        return duration_;
    }
    void PlayerViewModel::Duration(double value) {
        if (value != duration_) {
            duration_ = value;
            propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Duration" });
        }
    }
    void PlayerViewModel::Index(uint32_t value) {
        if (value != index_) {
            index_ = value;
            propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Index" });
        }
    }
    void PlayerViewModel::Volume(double value)
    {
        if (value != volume_) {
            volume_ = value;
            propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Volume" });
        }
    }
    winrt::hstring PlayerViewModel::DoubleToString(double value) {
        return fast_io::wconcat_winrt_hstring(value);
    }
    winrt::Data::Library PlayerViewModel::Library() {
        return library_;
    }
    void PlayerViewModel::Library(winrt::Data::Library const& value) {
        if (value.name != library_.name) {
            library_ = value;
            // propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Library" });
        }
    }
    winrt::hstring PlayerViewModel::DurationToString(double value) {
         auto const hms{ std::chrono::hh_mm_ss{std::chrono::nanoseconds{static_cast<int64_t>(value) * 100}} };
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
