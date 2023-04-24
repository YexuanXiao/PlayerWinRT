#include "pch.h"
#include "LibraryInfo.h"
#if __has_include("LibraryInfo.g.cpp")
#include "LibraryInfo.g.cpp"
#endif

namespace winrt::Player::implementation
{
    LibraryInfo::LibraryInfo(hstring const& library, hstring const& protocol, hstring const& address, hstring const& icon) :library_(library), protocol_(protocol), address_(address), icon_(icon) {
    }

    hstring LibraryInfo::Library() {
        return library_;
    }
    void LibraryInfo::Library(hstring const& value) {
        if (library_ != value)
        {
            library_ = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Library" });
        }
    }

    hstring LibraryInfo::Protocol() {
        return protocol_;
    }

    void LibraryInfo::Protocol(hstring const& value) {
        if (protocol_ != value)
        {
            protocol_ = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Protocol" });
        }
    }
    hstring LibraryInfo::Address() {
        return address_;
    }

    void LibraryInfo::Address(hstring const& value) {
        if (address_ != value)
        {
            address_ = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Address" });
        }
    }
    hstring LibraryInfo::Icon() {
        return icon_;
    }

    void LibraryInfo::Icon(hstring const& value) {
        if (icon_ != value)
        {
            icon_ = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Icon" });
        }
    }

    winrt::event_token LibraryInfo::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }

    void LibraryInfo::PropertyChanged(winrt::event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
