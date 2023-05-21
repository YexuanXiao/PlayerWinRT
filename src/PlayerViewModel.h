#pragma once
#include "PlayerViewModel.g.h"

namespace winrt::Player::implementation
{
    struct PlayerViewModel : PlayerViewModelT<PlayerViewModel>
    {
        PlayerViewModel();
        double Volume();
        void Volume(double value);
        uint32_t Index();
        void Index(uint32_t value);
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
        static winrt::hstring DoubleToString(double);
    private:
        double volume_;
        uint32_t index_;
        winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
    };
}
namespace winrt::Player::factory_implementation
{
    struct PlayerViewModel : PlayerViewModelT<PlayerViewModel, implementation::PlayerViewModel>
    {
    };
}
