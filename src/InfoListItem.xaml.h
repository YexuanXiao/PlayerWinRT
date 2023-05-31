#pragma once

#include "InfoListItem.g.h"

namespace winrt::Player::implementation
{
    struct InfoListItem: InfoListItemT<InfoListItem>
    {
        InfoListItem();

        winrt::hstring Track();
        winrt::hstring Title();
        winrt::hstring Artist();
        winrt::hstring State();
        winrt::hstring Bitrate();
        winrt::hstring Duration();

        void Track(winrt::hstring const&);
        void Title(winrt::hstring const&);
        void Artist(winrt::hstring const&);
        void State(winrt::hstring const&);
        void Bitrate(winrt::hstring const&);
        void Duration(winrt::hstring const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct InfoListItem: InfoListItemT<InfoListItem, implementation::InfoListItem>
    {
    };
}
