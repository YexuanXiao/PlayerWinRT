#pragma once

#include "InfoViewModel.g.h"

namespace winrt::Player::implementation
{
    struct InfoViewModel: InfoViewModelT<InfoViewModel>
    {
        InfoViewModel() = default;
        InfoViewModel(winrt::Data::Music const& info);

        winrt::hstring Title() const;
        winrt::hstring Album() const;
        winrt::hstring Genre() const;
        winrt::hstring Artist() const;
        winrt::hstring Albumartist() const;
        winrt::hstring Path() const;
        int64_t Duration() const;
        uint32_t Year() const;
        uint32_t Bitrate() const;
        uint32_t Track() const;
        winrt::hstring State() const;

        winrt::Data::Music Get() const;
        void SetState(bool state);

        static winrt::hstring TransformBitrate(uint32_t);
        static winrt::hstring TransformDuration(int64_t);
        static winrt::hstring DecisionTitle(winrt::hstring const&, winrt::hstring const&);
        static const winrt::hstring& DecisionArtist(winrt::hstring const&, winrt::hstring const&);

        void PropertyChanged(winrt::event_token const&) noexcept;
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const&);

    private:
        bool is_active_{ false };
        winrt::Data::Music info_{};
        winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_{};

        static const inline winrt::hstring EmptyString{};
        static const inline winrt::hstring NoiseCancelation{ L"\uF61F" };
    };
}

namespace winrt::Player::factory_implementation
{
    struct InfoViewModel: InfoViewModelT<InfoViewModel, implementation::InfoViewModel>
    {
    };
}
