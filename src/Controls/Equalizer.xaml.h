#pragma once

#include "Equalizer.g.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Media.PlayBack.h>

namespace winrt::Player::implementation
{
    struct Equalizer : EqualizerT<Equalizer>
    {
        Equalizer(winrt::Windows::Media::Playback::MediaPlayer);
    private:
        winrt::Windows::Media::Playback::MediaPlayer player_{ nullptr };
    public:
        winrt::Windows::Foundation::IAsyncAction ContentDialog_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct Equalizer : EqualizerT<Equalizer, implementation::Equalizer>
    {
    };
}
