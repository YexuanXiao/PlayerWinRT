#pragma once

#include "LibraryInfo.g.h"

namespace winrt::Player::implementation
{
    struct LibraryInfo : LibraryInfoT<LibraryInfo>
    {
        LibraryInfo() = default;
        LibraryInfo(winrt::hstring const&, winrt::hstring const&, winrt::hstring const&, winrt::hstring const&);
        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(winrt::event_token const& token) noexcept;
        winrt::hstring Library();
        void Library(winrt::hstring const&);
        winrt::hstring Protocol();
        void Protocol(winrt::hstring const&);
        winrt::hstring Address();
        void Address(winrt::hstring const&);
        winrt::hstring Icon();
        void Icon(winrt::hstring const&);
    private:
        winrt::hstring library_;
        winrt::hstring protocol_;
        winrt::hstring address_;
        winrt::hstring icon_;
        winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::Player::factory_implementation
{
    struct LibraryInfo : LibraryInfoT<LibraryInfo, implementation::LibraryInfo>
    {
    };
}
