#include "pch.h"
#include "PlayerViewModel.h"
#include "PlayerViewModel.g.cpp"

#include "SettingsHelper.h"

namespace winrt::Player::implementation
{
    PlayerViewModel::PlayerViewModel() {
        volume_ = SettingsHelper::GetVolume();
    }
    double PlayerViewModel::Volume()
    {
        return volume_;
    }
    uint32_t PlayerViewModel::Index() {
        return index_;
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
    winrt::event_token PlayerViewModel::PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChanged_.add(handler);
    }
    void PlayerViewModel::PropertyChanged(winrt::event_token const& token) noexcept
    {
        propertyChanged_.remove(token);
    }
}
