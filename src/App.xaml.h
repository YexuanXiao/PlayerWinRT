#pragma once

#include "App.xaml.g.h"

namespace winrt::Player::implementation
{
    struct App : AppT<App>
    {
        App();
        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
    private:
        // lazy initialization, use nullptr constructor
        // and initialize at other time
        // or use default constructor or other constructor
        Microsoft::UI::Xaml::Window window_{ nullptr };
        // use for disable multi-instance, keep A-Za-z, and no '\'
        std::wstring_view appname{L"PlayerWinRT"};

        void AppWindow_Changed(Microsoft::UI::Windowing::AppWindow, Microsoft::UI::Windowing::AppWindowChangedEventArgs const&);
        void AppTitleBar_Loaded(IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void AppTitleBar_SizeChanged(IInspectable const&, Microsoft::UI::Xaml::SizeChangedEventArgs const&);
        void SetDragRegionForCustomTitleBar();
    };
}
