#include "pch.h"
#include "Welcome.xaml.h"
#if __has_include("Welcome.g.cpp")
#include "Welcome.g.cpp"
#endif

#include <Win32Helper.h>
#include <SettingsHelper.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

using namespace Windows::Foundation;

namespace winrt::Player::implementation
{
    Welcome::Welcome()
    {
        InitializeComponent();
    }
    IAsyncAction Welcome::MusicFolder_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
        auto theme{ ActualTheme() };
        auto dialog{ ContentDialog{} };
        dialog.XamlRoot(XamlRoot());
        dialog.Title(winrt::box_value(resourceLoader.GetString(L"AddLibrary/Content")));
        dialog.PrimaryButtonText(resourceLoader.GetString(L"Add"));
        dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
        dialog.DefaultButton(ContentDialogButton::Close);
        auto musicFolder{ Windows::Storage::KnownFolders::MusicLibrary() };
        auto page{ Player::EditLibrary{musicFolder.DisplayName(),resourceLoader.GetString(L"Local/Text"), Win32Helper::GetMusicFolderPath(), L"\uE770"} };
        dialog.Content(page);
        dialog.RequestedTheme(theme);
        auto result{ co_await dialog.ShowAsync() };
        // use return value
    }
    IAsyncAction Welcome::OtherFolder_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
        auto theme{ ActualTheme() };
        auto dialog{ ContentDialog{} };
        dialog.XamlRoot(XamlRoot());
        dialog.Title(winrt::box_value(resourceLoader.GetString(L"AddLibrary/Content")));
        dialog.PrimaryButtonText(resourceLoader.GetString(L"Add"));
        dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
        dialog.DefaultButton(ContentDialogButton::Close);
        auto page{ Player::EditLibrary{} };
        dialog.Content(page);
        dialog.RequestedTheme(theme);
        auto result{ co_await dialog.ShowAsync() };
        // use return value
    }


    void Welcome::Theme_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SettingsHelper::SetTheme(XamlRoot(), ElementTheme::Dark);
    }

}