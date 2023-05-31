// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "NowPlaying.xaml.h"
#if __has_include("NowPlaying.g.cpp")
#include "NowPlaying.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Player::implementation
{
    NowPlaying::NowPlaying()
    {
        InitializeComponent();
    }

    int32_t NowPlaying::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void NowPlaying::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void NowPlaying::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }
}
