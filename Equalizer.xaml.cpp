// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "Equalizer.xaml.h"
#if __has_include("Equalizer.g.cpp")
#include "Equalizer.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Player::implementation
{
    Equalizer::Equalizer()
    {
        InitializeComponent();
    }
}
