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
    void PlayerViewModel::Volume(double value)
    {
        volume_ = value;
        SettingsHelper::SetVolume(volume_);
        propertyChanged_(*this, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{ L"Volume" });
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
