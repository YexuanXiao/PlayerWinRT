#include "pch.h"
#include "EditLibrary.xaml.h"
#if __has_include("EditLibrary.g.cpp")
#include "EditLibrary.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    EditLibrary::EditLibrary()
    {
        InitializeComponent();
    }
}
