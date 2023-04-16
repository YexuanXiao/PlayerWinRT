#include "pch.h"
#include "RootPage.xaml.h"
#if __has_include("RootPage.g.cpp")
#include "RootPage.g.cpp"
#endif

#include "EditLibrary.xaml.h"
#include "About.xaml.h"
#include "MusicInfo.xaml.h"
#include "Settings.xaml.h"

#include "SettingsHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Input;

using namespace Windows::Foundation;

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
        auto theme{ SettingsHelper::LoadTheme() };
        auto xamlRoot{ XamlRoot() };
        SettingsHelper::SetTheme(xamlRoot, theme);
    }
    IAsyncAction RootPage::Navigation_ItemInvoked(NavigationView const&, NavigationViewItemInvokedEventArgs const& args) {
        if (args.IsSettingsInvoked()) {
            rootFrame().Navigate(winrt::xaml_typename<Player::Settings>());
        }
        else [[likely]] {
            auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
            auto tagName{ winrt::unbox_value<winrt::hstring>(
                args.InvokedItemContainer().Tag()) };
            auto theme{ ActualTheme() };
            if (tagName == L"about") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(resourceLoader.GetString(L"About/Content")));
                dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
                dialog.DefaultButton(ContentDialogButton::Close);
                auto page{ Player::About{} };
                dialog.Content(page);
                dialog.RequestedTheme(theme);
                static_cast<void>(co_await dialog.ShowAsync());
            }
            else if(tagName==L"add") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(resourceLoader.GetString(L"EditLibrary/Content")));
                dialog.PrimaryButtonText(resourceLoader.GetString(L"Add"));
                dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
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
                dialog.Title(winrt::box_value(resourceLoader.GetString(L"Equalizer/Content")));
                dialog.PrimaryButtonText(resourceLoader.GetString(L"Save"));
                dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
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
        dialog.RequestedTheme(ActualTheme());
        static_cast<void>(co_await dialog.ShowAsync());
    }
    Frame RootPage::GetRootFrame() {
        return rootFrame();
    }
    /// <summary>
    ///  RootPage::GetAppTitleBar has two functions: 1. To be compatible with Windows 10,
    /// it needs to get the title bar element for Window::SetTitleBar;
    /// 2. It calculates the draggable area through the AppTitleBar element itself.
    /// If no need to compable with Windows 10, this function can be changed to GetDragRectangles,
    /// directly obtains the draggable area.
    /// </summary>
    Grid RootPage::GetAppTitleBar() {
        return AppTitleBar();
    }
}