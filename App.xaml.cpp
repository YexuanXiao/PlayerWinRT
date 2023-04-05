// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "MainPage.xaml.h"
#include "RootPage.xaml.h"
// for ::IWindowNative
#include <Microsoft.UI.Xaml.Window.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Player;
using namespace Player::implementation;

using namespace std::literals;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

    MakeAppUnique();

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
/// make app only have one instance
/// </summary>
void App::MakeAppUnique() {
    // use user's temp folder name as part of mutex name
    auto constexpr extra{ L"PlayerWinMutex\0"sv };
    // name buffer
    auto name{ std::to_array<wchar_t, MAX_PATH + 2 + extra.size() + 1>({}) };
    // get tmp folder, not guaranteed path availablity
    auto length{ GetTempPathW(static_cast<DWORD>(name.size()), &name[0]) };
    // add extra to path
    std::memcpy(&name[length], extra.data(), extra.size() + 1);

    // if mutex is held by another instance, NO NEED TO USE GetLastError
    if (!::CreateMutexExW(nullptr, &name[0], CREATE_MUTEX_INITIAL_OWNER, NULL)) {
        // enumerate all direct child windows of desktop
        for (auto pre{ ::FindWindowExW(nullptr, nullptr, nullptr, nullptr) };
            // return null if at the end
            pre != nullptr;
            // pass the pre as the 2nd argument to get next handle
            pre = ::FindWindowExW(nullptr, pre, nullptr, nullptr))
        {
            // check if window has the specified property
            // set in MainWindow constructor
            if (::GetPropW(pre, L"PlayerWinRT")) {
                // show and restore window
                ::ShowWindow(pre, SW_RESTORE);
                // activate, set foreground and get forcus
                ::SetForegroundWindow(pre);
                // exit app, DO NOT USE this->EXIT BECAUSE NOT CONSTRUCTED
                ::ExitProcess(1u);
            }
        }
    }
}
/// <summary>
/// C++ version that is almost 100% imitation of the C# version of the WinUI3 Gallery.
/// Create or get the rootFrame.
/// </summary>
Frame App::GetRootFrame() {

    Frame rootFrame{ nullptr };

    Player::RootPage rootPage = window.Content().try_as<Player::RootPage>();

    if (!rootPage) {
        rootPage = Player::RootPage();
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
    window = Window();

    HWND hWnd;
    window.as<::IWindowNative>()->get_WindowHandle(&hWnd);

    // make app only have one instance
    window.Activated([this, hWnd](IInspectable const&, WindowActivatedEventArgs const&) {
        // set window property to find instance
        ::SetPropW(hWnd, L"PlayerWinRT", hWnd);
        });
    
    // make root Frame and Main Page
    Frame rootFrame = GetRootFrame();

    // use custom title bar and make title bar draggable
    window.ExtendsContentIntoTitleBar(true);
    window.SetTitleBar(window.Content().as<Player::RootPage>().FindName(L"AppTitleBar").as<Border>());

    window.Activate();
}

void App::OnNavigationFailed(IInspectable const&, NavigationFailedEventArgs const& e)
{
    throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}