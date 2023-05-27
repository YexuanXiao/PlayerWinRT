#include "pch.h"
#include "Equalizer.xaml.h"
#if __has_include("Equalizer.g.cpp")
#include "Equalizer.g.cpp"
#endif

#include <winrt/Windows.ApplicationModel.Resources.h>

namespace winrt::Player::implementation
{
    Equalizer::Equalizer(winrt::Windows::Media::Playback::MediaPlayer player): player_(player)
    {
        InitializeComponent();
        auto resource{ winrt::Windows::ApplicationModel::Resources::ResourceLoader{} };
        Title(winrt::box_value(resource.GetString(L"Equalizer/Content")));
        PrimaryButtonText(resource.GetString(L"Save"));
        CloseButtonText(resource.GetString(L"Cancel"));
    }
    winrt::Windows::Foundation::IAsyncAction Equalizer::ContentDialog_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        co_return;
    }
}

/*
* https://learn.microsoft.com/en-us/answers/questions/1280085/how-can-i-add-equalizer-effect-to-the-mediaplayer
https://learn.microsoft.com/zh-cn/uwp/api/windows.media.audio.reverbeffectdefinition?view=winrt-22621
https://learn.microsoft.com/zh-cn/uwp/api/windows.media.audio.limitereffectdefinition?view=winrt-22621
https://learn.microsoft.com/zh-cn/uwp/api/windows.media.audio.echoeffectdefinition?view=winrt-22621
*/

