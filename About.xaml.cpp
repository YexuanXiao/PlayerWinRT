// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "About.xaml.h"
#if __has_include("About.g.cpp")
#include "About.g.cpp"
#endif

#include <Windows.Foundation.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Player::implementation
{
    About::About()
    {
        InitializeComponent();
        auto version{ Windows::ApplicationModel::Package::Current().Id().Version() };
        auto str{ std::wostringstream{} };
        // todo: use fastio
        str << version.Major << '.' << version.Minor << '.' << version.Build << '.' << version.Revision;
        Version().Text(winrt::hstring{str.view()});
    }
}
