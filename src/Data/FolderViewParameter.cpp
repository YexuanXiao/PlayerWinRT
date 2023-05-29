#include "pch.h"
#include "FolderViewParameter.h"
#if __has_include("FolderViewParameter.g.cpp")
#include "FolderViewParameter.g.cpp"
#endif

namespace winrt::Data::implementation
{
    FolderViewParameter::FolderViewParameter(winrt::Player::PlayerViewModel const& playerViewModel, winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> const& music, winrt::Windows::Media::Playback::MediaPlaybackList const& list, winrt::Data::Library const& library) : playerViewModel_(playerViewModel), music_(music), play_list_(list), library_(library)
    {
    }

    winrt::Player::PlayerViewModel FolderViewParameter::PlayerViewModel()
    {
        return playerViewModel_;
    }

    winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> FolderViewParameter::Music()
    {
        return music_;
    }

    winrt::Windows::Media::Playback::MediaPlaybackList FolderViewParameter::List()
    {
        return play_list_;
    }

    winrt::Data::Library FolderViewParameter::Library()
    {
        return library_;
    }
}
