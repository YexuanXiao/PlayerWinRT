#pragma once

#include "PlayerViewModel.g.h"
#include "SettingsHelper.h"

namespace winrt::Player::implementation
{
    struct PlayerViewModel: PlayerViewModelT<PlayerViewModel>
    {
        PlayerViewModel() = default;
        double Volume() const;
        uint32_t Index() const;
        double Position() const;
        double Duration() const;
        winrt::hstring Title() const;
        winrt::Data::Library Library() const;

        void Volume(double value);
        void Index(uint32_t value);
        void Position(double value);
        void Duration(double value);
        void Title(winrt::hstring const&);
        void Library(winrt::Data::Library const&);

        static winrt::hstring DoubleToString(double);
        static winrt::hstring DurationToString(double);

        void PropertyChanged(winrt::event_token const&) noexcept;
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const&);

    private:
        uint32_t index_;
        winrt::hstring title_{
#ifdef _DEBUG
            L"PlayerWinRT Dev"
#else
            L"PlayerWinRT"
#endif
        };
        double position_{ 0. };
        double duration_{ 0. };
        winrt::Data::Library library_{};
        double volume_{ SettingsHelper::GetVolume() };
        winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_{};
    };
}

namespace winrt::Player::factory_implementation
{
    struct PlayerViewModel: PlayerViewModelT<PlayerViewModel, implementation::PlayerViewModel>
    {
    };
}
