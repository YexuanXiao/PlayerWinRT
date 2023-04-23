#include "pch.h"
#include "Welcome.xaml.h"
#if __has_include("Welcome.g.cpp")
#include "Welcome.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    Welcome::Welcome()
    {
        InitializeComponent();
    }
}
