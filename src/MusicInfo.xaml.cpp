#include "pch.h"
#include "MusicInfo.xaml.h"
#if __has_include("MusicInfo.g.cpp")
#include "MusicInfo.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    MusicInfo::MusicInfo()
    {
        InitializeComponent();
    }
}

/*

https://learn.microsoft.com/zh-cn/uwp/api/windows.media.musicdisplayproperties?view=winrt-22621

*/