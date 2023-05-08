#include "pch.h"
#include "Progress.xaml.h"
#if __has_include("Progress.g.cpp")
#include "Progress.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    Progress::Progress()
    {
        InitializeComponent();
    }
}
