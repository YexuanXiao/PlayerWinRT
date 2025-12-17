#include "pch.h"
#include "About.xaml.h"
#if __has_include("About.g.cpp")
#include "About.g.cpp"
#endif

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Resources.h>

namespace winrt::Player::implementation
{
    About::About()
    {
        InitializeComponent();

        auto resource{ winrt::Windows::ApplicationModel::Resources::ResourceLoader{} };
        Title(winrt::box_value(resource.GetString(L"About/Content")));
        CloseButtonText(resource.GetString(L"Close"));
        auto version{ winrt::Windows::ApplicationModel::Package::Current().Id().Version() };
        Version().Text(util::hformat(L"{}.{}.{}", version.Build, version.Major, version.Minor, version.Revision));
    }
}
