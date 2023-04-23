#include "pch.h"
#include "PlayList.xaml.h"
#if __has_include("PlayList.g.cpp")
#include "PlayList.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    PlayList::PlayList()
    {
        InitializeComponent();
    }
}
