#include "pch.h"
#include "FolderViewParamater.h"
#if __has_include("FolderViewParamater.g.cpp")
#include "FolderViewParamater.g.cpp"
#endif

namespace winrt::Data::implementation
{
    FolderViewParamater::FolderViewParamater(winrt::Player::PlayerViewModel const& playerViewModel, winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> const& music, winrt::Windows::Media::Playback::MediaPlaybackList const& list, winrt::Data::Library const& library): playerViewModel_(playerViewModel), music_(music), list_(list), library_(library) {
    }

    winrt::Player::PlayerViewModel FolderViewParamater::PlayerViewModel()
    {
        return playerViewModel_;
    }
    winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> FolderViewParamater::Music()
    {
        return music_;
    }
    winrt::Windows::Media::Playback::MediaPlaybackList FolderViewParamater::List()
    {
        return list_;
    }
    winrt::Data::Library FolderViewParamater::Library()
    {
        return library_;
    }
}
