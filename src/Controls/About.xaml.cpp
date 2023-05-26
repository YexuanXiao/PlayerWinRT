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
        auto dot{ fast_io::mnp::chvw('.') };
        Version().Text(fast_io::wconcat_winrt_hstring(version.Major,dot,version.Major,dot,version.Minor,dot,version.Revision));
    }
}
