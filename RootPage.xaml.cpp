// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "RootPage.xaml.h"
#if __has_include("RootPage.g.cpp")
#include "RootPage.g.cpp"
#endif

#include "EditLibrary.xaml.h"
#include "About.xaml.h"

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
    void RootPage::Navigation_Loaded(IInspectable const&, RoutedEventArgs const&) {
        // you can also add items in code behind
    }
    IAsyncAction RootPage::Navigation_ItemInvoked(NavigationView, NavigationViewItemInvokedEventArgs args) {
        if (args.IsSettingsInvoked()) {
        }
        else {
            auto tagname{ winrt::unbox_value<winrt::hstring>(
                args.InvokedItemContainer().Tag()) };
            if (tagname == L"add") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(L"Add Library"));
                dialog.PrimaryButtonText(L"Add");
                dialog.CloseButtonText(L"Cancel");
                dialog.DefaultButton(ContentDialogButton::Close);
                auto page{ Player::EditLibrary{} };
                dialog.Content(page);
                auto result{ co_await dialog.ShowAsync() };
                // use return value
            }
            else if(tagname==L"about") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(L"About"));
                dialog.CloseButtonText(L"Cancel");
                dialog.DefaultButton(ContentDialogButton::Close);
                auto page{ Player::About{} };
                dialog.Content(page);
                static_cast<void>(co_await dialog.ShowAsync());
            }
        }
    }
    void RootPage::Navigate(NavigationViewItem item) {

    }
}
