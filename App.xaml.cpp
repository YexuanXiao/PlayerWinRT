// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "RootPage.xaml.h"
// for ::IWindowNative
#include <Microsoft.UI.Xaml.Window.h>

#include "Win32Helper.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI;
using namespace Microsoft::UI::Windowing;
using namespace Microsoft::UI::Xaml;
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
/// C++ version that is almost 100% imitation of the C# version of the WinUI3 Gallery.
/// Create or get the rootFrame.
/// </summary>
Frame App::GetRootFrame() {

    auto rootFrame{ Frame{ nullptr } };
    auto rootPage{ window.Content().try_as<Player::RootPage>() };

    if (!rootPage) {
        rootPage = Player::RootPage{};
        rootFrame = rootPage.FindName(L"rootFrame").try_as<Frame>();
        // show how to Navigate rootFrame.Navigate(xaml_typename<Player::MainPage>());
        if(!rootFrame) throw hresult_class_not_available(L"Root frame not found");
        rootFrame.NavigationFailed({ this,&App::OnNavigationFailed });
        window.Content(rootPage);
    }
    else {
        rootFrame = rootPage.FindName(L"rootFrame").as<Frame>();
    }

    return rootFrame;
}

/// <summary>
/// Invoked when the application is launched.
/// Create and activate the window.
/// </summary>
/// <param name="e">
/// Almost useless.
/// Unless truely know what LaunchActivatedEventArgs have,
/// absolutely don't need to use it.
/// </param>
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    // lazy initialize
    window = Window{};

    // make rootPage
    auto rootPage{ window.Content().try_as<Player::RootPage>() };
    if (!rootPage) {
        rootPage = Player::RootPage{};
        auto rootFrame{ rootPage.GetRootFrame() };
        rootFrame.NavigationFailed({ this,&App::OnNavigationFailed });
        window.Content(rootPage);
    }

    // use custom title bar and make title bar draggable
    window.ExtendsContentIntoTitleBar(true);
    window.SetTitleBar(window.Content().as<Player::RootPage>().FindName(L"AppTitleBar").as<Border>());
    window.Title(appname);

    // todo: use appwindow instead of window
    // https://zhuanlan.zhihu.com/p/603180596

    HWND hWnd;
    window.as<::IWindowNative>()->get_WindowHandle(&hWnd);

    // make app only have one instance
    window.Activated([this, hWnd](IInspectable const&, WindowActivatedEventArgs const&) {
        // set window property to find instance
        Win32Helper::DisableMultiInstanceWindow(hWnd, appname);
        });

    // set window icon
    auto wndID = ::GetWindowIdFromWindow(hWnd);
    auto appwindow = AppWindow::GetFromWindowId(wndID);
    // https://www.aconvert.com/cn/icon/png-to-ico/
    appwindow.SetIcon(L"Assets/PlayerWinRT.ico");

    window.Activate();
}

void App::OnNavigationFailed(IInspectable const&, NavigationFailedEventArgs const& e)
{
    throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}