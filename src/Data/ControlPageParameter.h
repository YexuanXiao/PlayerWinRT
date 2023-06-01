#pragma once

#include "ControlPageParameter.g.h"

namespace winrt::Data::implementation
{
    struct ControlPageParameter: ControlPageParameterT<ControlPageParameter>
    {
        ControlPageParameter(winrt::Player::PlayerViewModel const&, winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> const&, winrt::Windows::Media::Playback::MediaPlaybackList const&, winrt::Data::Library const&);

        winrt::Player::PlayerViewModel PlayerViewModel();
        winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> Music();
        winrt::Windows::Media::Playback::MediaPlaybackList List();
        winrt::Data::Library Library();

    private:
        winrt::Player::PlayerViewModel player_view_model_;
        winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> music_;
        winrt::Windows::Media::Playback::MediaPlaybackList play_list_;
        winrt::Data::Library library_;
    };
}

namespace winrt::Data::factory_implementation
{
    struct ControlPageParameter: ControlPageParameterT<ControlPageParameter, implementation::ControlPageParameter>
    {
    };
}
