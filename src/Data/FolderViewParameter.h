#pragma once
#include "FolderViewParameter.g.h"

namespace winrt::Data::implementation
{
    struct FolderViewParameter : FolderViewParameterT<FolderViewParameter>
    {
        FolderViewParameter(winrt::Player::PlayerViewModel const&, winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> const&, winrt::Windows::Media::Playback::MediaPlaybackList const&, winrt::Data::Library const&);

        winrt::Player::PlayerViewModel PlayerViewModel();
        winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> Music();
        winrt::Windows::Media::Playback::MediaPlaybackList List();
        winrt::Data::Library Library();

    private:
        winrt::Player::PlayerViewModel playerViewModel_;
        winrt::Windows::Foundation::Collections::IVector<winrt::Data::Music> music_;
        winrt::Windows::Media::Playback::MediaPlaybackList list_;
        winrt::Data::Library library_;
    };
}

namespace winrt::Data::factory_implementation
{
    struct FolderViewParameter : FolderViewParameterT<FolderViewParameter, implementation::FolderViewParameter>
    {
    };
}
