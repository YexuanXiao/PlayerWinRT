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

namespace winrt::Player::implementation
{
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
        // lazy initialize
        window_ = Microsoft::UI::Xaml::Window{};

        // make rootPage
        auto rootPage{ window_.Content().try_as<Player::RootPage>() };
        if (!rootPage) {
            rootPage = Player::RootPage{};
            auto rootFrame{ rootPage.GetRootFrame() };
            window_.Content(rootPage);
        }

        // use custom title bar and make title bar draggable
        window_.ExtendsContentIntoTitleBar(true);
        window_.SetTitleBar(window_.Content().as<Player::RootPage>().FindName(L"AppTitleBar").as<Border>());
        window_.Title(appname);

        // todo: use appwindow instead of window_
        // https://zhuanlan.zhihu.com/p/603180596

        HWND hWnd;
        window_.as<::IWindowNative>()->get_WindowHandle(&hWnd);

        // make app only have one instance
        window_.Activated([this, hWnd](IInspectable const&, WindowActivatedEventArgs const&) {
            // set window_ property to find instance
            Win32Helper::DisableMultiInstanceWindow(hWnd, appname);
            });

        // set window_ icon
        auto wndID = ::GetWindowIdFromWindow(hWnd);
        auto appwindow = AppWindow::GetFromWindowId(wndID);
        // https://www.aconvert.com/cn/icon/png-to-ico/
        appwindow.SetIcon(L"Assets/PlayerWinRT.ico");

        window_.Activate();
    }
}