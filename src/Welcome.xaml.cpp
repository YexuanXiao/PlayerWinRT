#include "pch.h"
#include "Welcome.xaml.h"
#if __has_include("Welcome.g.cpp")
#include "Welcome.g.cpp"
#endif

#include "Win32Helper.h"
#include "SettingsHelper.h"
#include "Data.h"

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
    IAsyncAction Welcome::AddLibrary_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        // show edit library dialog
        auto dialog{ LibraryEditor{nullptr} };
        auto const resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };

        if (winrt::unbox_value<winrt::hstring>(sender.try_as<Button>().Tag()) == L"music") [[likely]] {
            auto musicFolder{ Windows::Storage::KnownFolders::MusicLibrary() };
            dialog = Player::LibraryEditor{ musicFolder.DisplayName(), resourceLoader.GetString(L"Local/Text"), Win32Helper::GetMusicFolderPath(), winrt::hstring{} };
        }
        else {
            dialog = Player::LibraryEditor{};
        }

        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }

    void Welcome::Theme_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        SettingsHelper::SetTheme(XamlRoot(), ElementTheme::Dark);
    }

}