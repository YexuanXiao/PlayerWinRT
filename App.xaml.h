// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "App.xaml.g.h"

namespace winrt::Player::implementation
{

    struct App : AppT<App>
    {
        App();
        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
        void OnNavigationFailed(IInspectable const&, Microsoft::UI::Xaml::Navigation::NavigationFailedEventArgs const&);

    private:
        void MakeAppUnique();
        Microsoft::UI::Xaml::Controls::Frame GetRootFrame();

    private:
        // lazy initialization, use nullptr constructor
        // and initialize at other time
        // or use default constructor or other constructor
        Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
