#include "pch.h"
#include "About.xaml.h"
#if __has_include("About.g.cpp")
#include "About.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    About::About()
    {
        InitializeComponent();

        auto version{ Windows::ApplicationModel::Package::Current().Id().Version() };
        Version().Text(fast_io::wconcat_winrt_hstring(version.Major,'.',version.Major,'.',version.Minor,'.',version.Revision));
    }
}
