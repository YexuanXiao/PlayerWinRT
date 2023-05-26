#include "pch.h"
#include "Equalizer.xaml.h"
#if __has_include("Equalizer.g.cpp")
#include "Equalizer.g.cpp"
#endif

#include <winrt/Windows.ApplicationModel.Resources.h>

namespace winrt::Player::implementation
{
    Equalizer::Equalizer()
    {
        InitializeComponent();
        auto resource{ winrt::Windows::ApplicationModel::Resources::ResourceLoader{} };
        Title(winrt::box_value(resource.GetString(L"Equalizer/Content")));
        PrimaryButtonText(resource.GetString(L"Save"));
        CloseButtonText(resource.GetString(L"Cancel"));
    }
}
