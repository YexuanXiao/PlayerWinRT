#pragma once

#include "App.xaml.g.h"

namespace winrt::Player::implementation
{
    struct App: AppT<App>
    {
        App();
        void OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        // use for disable multi-instance, keep A-Za-z, and no '\'
        std::wstring_view appname_{ L"PlayerWinRT" };
        // lazy initialization, use nullptr constructor
        // and initialize at other time
        // or use default constructor or other constructor
        winrt::Player::RootPage root_page_{ nullptr };
        winrt::Microsoft::UI::Xaml::Window window_{ nullptr };

        void SetDragRegionForCustomTitleBar();
        void AppTitleBar_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void AppWindow_Changed(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const&);
        void AppTitleBar_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const&);
        static void SetTitleBarTheme(winrt::Microsoft::UI::Windowing::AppWindowTitleBar& titlebar, winrt::Microsoft::UI::Xaml::ElementTheme theme);
    };
}
