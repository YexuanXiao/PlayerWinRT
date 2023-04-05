// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "RootPage.xaml.h"
#if __has_include("RootPage.g.cpp")
#include "RootPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Input;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Player::implementation
{
    RootPage::RootPage()
    {
        InitializeComponent();
        for (auto i : MainLibraryList().MenuItems()) {
        }
    }
    hstring RootPage::AppTitleText() {
#if defined _DEBUG
        return L"PlayerWinRT Dev";
#else
        return L"PlayerWinRT";
#endif
    }
    void RootPage::AppTitleText(hstring const&) {
    }
}
