#include "pch.h"
#include "ControlPageParameter.h"
#if __has_include("ControlPageParameter.g.cpp")
#include "ControlPageParameter.g.cpp"
#endif

namespace winrt::Data::implementation
{
    ControlPageParameter::ControlPageParameter(winrt::Player::PlayerViewModel const& playerViewModel, winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> const& music, winrt::Windows::Media::Playback::MediaPlaybackList const& list, winrt::Data::Library const& library) : player_view_model_(playerViewModel), music_(music), play_list_(list), library_(library)
    {
    }

    winrt::Player::PlayerViewModel ControlPageParameter::PlayerViewModel()
    {
        return player_view_model_;
    }

    winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> ControlPageParameter::Music()
    {
        return music_;
    }

    winrt::Windows::Media::Playback::MediaPlaybackList ControlPageParameter::List()
    {
        return play_list_;
    }

    winrt::Data::Library ControlPageParameter::Library()
    {
        return library_;
    }
}
