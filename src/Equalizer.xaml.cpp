#include "pch.h"
#include "Equalizer.xaml.h"
#if __has_include("Equalizer.g.cpp")
#include "Equalizer.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    Equalizer::Equalizer()
    {
        InitializeComponent();
    }
}
