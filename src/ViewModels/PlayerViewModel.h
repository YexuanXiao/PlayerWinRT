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
        double Position();
        void Position(double value);
        double Duration();
        void Duration(double value);
        winrt::Data::Library Library();
        void Library(winrt::Data::Library const&);
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
        static winrt::hstring DoubleToString(double);
        static winrt::hstring DurationToString(double);
    private:
        double volume_;
        uint32_t index_;
        double position_;
        double duration_;
        winrt::Data::Library library_;
        winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
    };
}
namespace winrt::Player::factory_implementation
{
    struct PlayerViewModel : PlayerViewModelT<PlayerViewModel, implementation::PlayerViewModel>
    {
    };
}
