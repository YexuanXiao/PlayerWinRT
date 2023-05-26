#include "pch.h"
#include "MusicInfo.xaml.h"
#if __has_include("MusicInfo.g.cpp")
#include "MusicInfo.g.cpp"
#endif

#include <winrt/Windows.ApplicationModel.Resources.h>

namespace winrt::Player::implementation
{
    MusicInfo::MusicInfo()
    {
        InitializeComponent();

        auto resource{ winrt::Windows::ApplicationModel::Resources::ResourceLoader{} };
        Title(winrt::box_value(resource.GetString(L"MusicInfo/Text")));
        CloseButtonText(resource.GetString(L"Close"));
    }
}

/*

https://learn.microsoft.com/zh-cn/uwp/api/windows.media.musicdisplayproperties?view=winrt-22621

*/