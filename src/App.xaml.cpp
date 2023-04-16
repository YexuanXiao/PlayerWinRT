#include "pch.h"

#include "App.xaml.h"
// for ::IWindowNative
#include <Microsoft.UI.Xaml.Window.h>

#include "Win32Helper.h"
#include "SettingsHelper.h"
#include "RootPage.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Graphics;
using namespace Microsoft::UI;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Windowing;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Player;
using namespace Player::implementation;

/// <summary>
/// Initializes the singleton application object. This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

    Win32Helper::DisableMultiInstanceEntry(appname, 1u);
    // alternate way
    // https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/migrate-to-windows-app-sdk/guides/applifecycle

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
}

/// <summary>
/// Invoked when the application is launched.
/// Create and activate the window_.
/// </summary>
/// <param name="e">
/// Almost useless.
/// Unless truely know what LaunchActivatedEventArgs have,
/// absolutely don't need to use it.
/// </param>
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    // lazy initialize window
    window_ = Window{};

    // make rootPage
    auto rootPage{ window_.Content().try_as<Player::RootPage>() };
    if (!rootPage) [[likely]] {
        rootPage = Player::RootPage{};
        auto rootFrame{ rootPage.GetRootFrame() };
        window_.Content(rootPage);
    }
    auto appTitleBar{ rootPage.GetAppTitleBar() };

    // initialize AppWindow
    auto wndId{ Win32Helper::GetWindowIdFromWindow(window_) };
    appWindow_ = AppWindow::GetFromWindowId(wndId);
    // https://www.aconvert.com/cn/icon/png-to-ico/
    appWindow_.SetIcon(L"Assets/PlayerWinRT.ico");

#define Windows10 false
    // test Windows 10 branch on Windows 11
    if (AppWindowTitleBar::IsCustomizationSupported() && !Windows10) [[likely]]

#undef Windows10
    {
        appWindow_.Changed({ this,&App::AppWindow_Changed });
        auto titleBar{ appWindow_.TitleBar() };
        titleBar.ExtendsContentIntoTitleBar(true);
        appTitleBar.Loaded({ this,&App::AppTitleBar_Loaded });
        appTitleBar.SizeChanged({ this, &App::AppTitleBar_SizeChanged });

        // set titlebar theme
        auto theme{ SettingsHelper::LoadTheme() };
        SettingsHelper::SetTitleBarTheme(titleBar, theme);

        rootPage.ActualThemeChanged([this](FrameworkElement const& sender, IInspectable const&) {

            auto titleBar{ appWindow_.TitleBar() };
            if (!titleBar.ExtendsContentIntoTitleBar())
                return;
            SettingsHelper::SetTitleBarTheme(titleBar, sender.ActualTheme());
            });
    }
    else
    {
        // In the case that title bar customization is not supported, fallback to WindowChrome
        window_.ExtendsContentIntoTitleBar(true);
        window_.SetTitleBar(appTitleBar);
    }

    // make app only have one instance
    window_.Activated([this](IInspectable const& sender, WindowActivatedEventArgs const&) {
        // set window_ property to find instance
        Win32Helper::DisableMultiInstanceWindow(sender.as<Window>(), appname);
        });

    window_.Activate();
}

void App::AppWindow_Changed(AppWindow sender, AppWindowChangedEventArgs const& args) {
    // todo: https://learn.microsoft.com/zh-cn/windows/apps/develop/title-bar?tabs=wasdk#full-customization-example
}
void App::AppTitleBar_Loaded(IInspectable const&, RoutedEventArgs const&) {
    assert(AppWindowTitleBar::IsCustomizationSupported());
    SetDragRegionForCustomTitleBar();
}
void App::AppTitleBar_SizeChanged(IInspectable const&, SizeChangedEventArgs const&) {
    assert(AppWindowTitleBar::IsCustomizationSupported());
    assert(appWindow_.TitleBar().ExtendsContentIntoTitleBar());
    SetDragRegionForCustomTitleBar();
}
double App::GetScaleAdjustment() {
    auto wndId{ Win32Helper::GetWindowIdFromWindow(window_) };
    auto displayArea{ DisplayArea::GetFromWindowId(wndId, DisplayAreaFallback::Primary) };
    auto hMonitor{ ::GetMonitorFromDisplayId(displayArea.DisplayId()) };
    auto dpiX{ Win32Helper::GetDpiXForMonitor(hMonitor) };
    auto scaleFactorPercent{ (dpiX * 100 + (96 >> 1)) / 96 };
    return scaleFactorPercent / 100.;
}
void App::SetDragRegionForCustomTitleBar() {
    assert(AppWindowTitleBar::IsCustomizationSupported());
    auto titleBar{ appWindow_.TitleBar() };
    assert(titleBar.ExtendsContentIntoTitleBar());
    auto scaleAdjustment{ GetScaleAdjustment() };
    auto appTitleBar{ window_.Content().as<Player::RootPage>().GetAppTitleBar() };
    auto rect{ RectInt32{ } };
    rect.X = static_cast<int32_t>((titleBar.LeftInset() + 48) * scaleAdjustment);
    rect.Y = 0;
    rect.Height = static_cast<int32_t>(appTitleBar.ActualHeight() * scaleAdjustment);
#if defined _DEBUG
    // make application tool bar clickable
    rect.Width = static_cast<int32_t>(appTitleBar.ActualWidth() * scaleAdjustment / 3);
#else
    rect.Width = appTitleBar.ActualWidth() * scaleAdjustment - rect.X - titleBar.RightInset();
#endif
    titleBar.SetDragRectangles(winrt::array_view(&rect, &rect + 1));
}
