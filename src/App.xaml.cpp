// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
// for ::IWindowNative
#include <Microsoft.UI.Xaml.Window.h>

#include "Win32Helper.h"

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
    if (!rootPage) {
        rootPage = Player::RootPage{};
        auto rootFrame{ rootPage.GetRootFrame() };
        window_.Content(rootPage);
    }

    HWND hWnd;
    window_.as<::IWindowNative>()->get_WindowHandle(&hWnd);

    // make app only have one instance
    window_.Activated([this, hWnd](IInspectable const&, WindowActivatedEventArgs const&) {
        // set window_ property to find instance
        Win32Helper::DisableMultiInstanceWindow(hWnd, appname);
        });

    // set window_ icon
    auto wndID = Microsoft::UI::GetWindowIdFromWindow(hWnd);
    auto appWindow = AppWindow::GetFromWindowId(wndID);
    // https://www.aconvert.com/cn/icon/png-to-ico/
    appWindow.SetIcon(L"Assets/PlayerWinRT.ico");

    // title bar
    auto titleBar{ appWindow.TitleBar() };
    titleBar.ExtendsContentIntoTitleBar(true);
    titleBar.ButtonBackgroundColor(Colors::Transparent());
    titleBar.ButtonInactiveBackgroundColor(Colors::Transparent());
    window_.SizeChanged([appWindow](IInspectable const&, WindowSizeChangedEventArgs const& args) {
        auto rect{ RectInt32{48, 0, static_cast<int32_t>(args.Size().Width) - 48, 48} };
        appWindow.TitleBar().SetDragRectangles(winrt::array_view(&rect, &rect + 1));
        });

    window_.Activate();
}