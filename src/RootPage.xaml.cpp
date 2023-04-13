// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "RootPage.xaml.h"
#if __has_include("RootPage.g.cpp")
#include "RootPage.g.cpp"
#endif

#include "EditLibrary.xaml.h"
#include "About.xaml.h"
#include "MusicInfo.xaml.h"
#include "Settings.xaml.h"

#include <SettingsHelper.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Input;

using namespace Windows::Foundation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Player::implementation
{
    RootPage::RootPage()
    {
        InitializeComponent();
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
    void RootPage::Navigation_Loaded(IInspectable const&, RoutedEventArgs const&) {
        // you can also add items in code behind
    }
    IAsyncAction RootPage::Navigation_ItemInvoked(NavigationView, NavigationViewItemInvokedEventArgs args) {
        if (args.IsSettingsInvoked()) {
            rootFrame().Navigate(winrt::xaml_typename<Player::Settings>());
        }
        else {
            auto tagName{ winrt::unbox_value<winrt::hstring>(
                args.InvokedItemContainer().Tag()) };
            auto theme{ SettingsHelper::LoadTheme() };
            if (tagName == L"about") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(L"About"));
                dialog.CloseButtonText(L"Cancel");
                auto page{ Player::About{} };
                dialog.Content(page);
                dialog.RequestedTheme(theme);
                static_cast<void>(co_await dialog.ShowAsync());
            }
            else if(tagName==L"add") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(L"Add Library"));
                dialog.PrimaryButtonText(L"Add");
                dialog.CloseButtonText(L"Cancel");
                dialog.DefaultButton(ContentDialogButton::Close);
                auto page{ Player::EditLibrary{} };
                dialog.Content(page);
                dialog.RequestedTheme(theme);
                auto result{ co_await dialog.ShowAsync() };
                // use return value
            }
            else if (tagName == L"equalizer") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(L"Equalizer"));
                dialog.PrimaryButtonText(L"Save");
                dialog.CloseButtonText(L"Cancel");
                dialog.DefaultButton(ContentDialogButton::Close);
                auto page{ Player::Equalizer{} };
                dialog.Content(page);
                dialog.RequestedTheme(theme);
                auto result{ co_await dialog.ShowAsync() };
                // use return value
            }
        }
    }
    IAsyncAction RootPage::MusicInfo_Click(IInspectable const&, RoutedEventArgs const& e)
    {
        auto dialog{ ContentDialog{} };
        dialog.XamlRoot(XamlRoot());
        dialog.Title(winrt::box_value(L"Music Info"));
        dialog.CloseButtonText(L"Close");
        auto page{ Player::MusicInfo{} };
        dialog.Content(page);
        static_cast<void>(co_await dialog.ShowAsync());
    }
    Frame RootPage::GetRootFrame() {
        return rootFrame();
    }
}