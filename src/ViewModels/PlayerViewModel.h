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
        winrt::hstring Album() const;
        winrt::hstring Artist() const;
        winrt::hstring AppTitle() const;
        winrt::Data::Library Library() const;
        winrt::Microsoft::UI::Xaml::Media::ImageSource Image() const;

        void Volume(double value);
        void Index(uint32_t value);
        void Position(double value);
        void Duration(double value);
        void Title(winrt::hstring const&);
        void Album(winrt::hstring const&);
        void Artist(winrt::hstring const&);
        void AppTitle(winrt::hstring const&);
        void Library(winrt::Data::Library const&);
        void Image(winrt::Microsoft::UI::Xaml::Media::ImageSource const&);

        static winrt::hstring DoubleToString(double);
        static winrt::hstring DurationToString(double);

        void PropertyChanged(winrt::event_token const&) noexcept;
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const&);

    private:
        uint32_t index_;
        winrt::hstring album_{ L"None" };
        winrt::hstring artist_{ L"Artist" };
        winrt::hstring title_{ L"PlayerWinRT" };
        double position_{ 0. };
        double duration_{ 0. };
        winrt::hstring app_title_{
#ifdef _DEBUG
            L"PlayerWinRT Dev"
#else
            L"PlayerWinRT"
#endif
        };
        winrt::Data::Library library_{};
        double volume_{ SettingsHelper::GetVolume() };
        winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_{};
        winrt::Microsoft::UI::Xaml::Media::ImageSource image_{ winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage{ winrt::Windows::Foundation::Uri{ L"ms-appx:///Assets/StoreLogo.scale-400.png" } } };
    };
}

namespace winrt::Player::factory_implementation
{
    struct PlayerViewModel: PlayerViewModelT<PlayerViewModel, implementation::PlayerViewModel>
    {
    };
}
