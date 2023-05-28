#pragma once
#include "PlayerViewModel.g.h"

namespace winrt::Player::implementation
{
    struct PlayerViewModel : PlayerViewModelT<PlayerViewModel>
    {
        PlayerViewModel();
        double Volume();
        uint32_t Index();
        double Position();
        double Duration();
        void Volume(double value);
        void Index(uint32_t value);
        void Position(double value);
        void Duration(double value);
        winrt::Data::Library Library();
        void Library(winrt::Data::Library const&);
        static winrt::hstring DoubleToString(double);
        static winrt::hstring DurationToString(double);
        void PropertyChanged(winrt::event_token const&) noexcept;
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const&);
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
