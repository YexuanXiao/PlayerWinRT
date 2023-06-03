#include "pch.h"
#include "Welcome.xaml.h"
#if __has_include("Welcome.g.cpp")
#include "Welcome.g.cpp"
#endif

#include "Win32Helper.h"
#include "SettingsHelper.h"
#include "Data.h"

namespace winrt::Player::implementation
{
    Welcome::Welcome()
    {
        InitializeComponent();

        auto items{ Languages().Items() };
        for (auto lang : winrt::Windows::Globalization::ApplicationLanguages::ManifestLanguages()) [[likely]]
        {
            auto item{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
            item.Text(SettingsHelper::GetLangTagName(lang));
            item.Tag(winrt::box_value(lang));
            item.Tapped(&Welcome::Language_Selected);
            items.Append(item);
        }
    }

    winrt::Windows::Foundation::IAsyncAction Welcome::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
    {
        if (args.Parameter() == nullptr)
            co_return;

        libraries_ = args.Parameter().try_as<winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library>>();
    }

    winrt::Windows::Foundation::IAsyncAction Welcome::AddLibrary_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        // show edit library dialog
        auto dialog{ LibraryEditor{ nullptr } };
        auto const resource{ winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };

        if (winrt::unbox_value<winrt::hstring>(sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>().Tag()) == L"music") [[likely]]
            dialog = winrt::Player::LibraryEditor{ libraries_, winrt::Windows::Storage::KnownFolders::MusicLibrary().DisplayName(), resource.GetString(L"Local/Text"), Win32Helper::GetMusicFolderPath(), winrt::hstring{} };
        else
            dialog = winrt::Player::LibraryEditor{ libraries_ };

        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }

    void Welcome::Theme_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        SettingsHelper::SetTheme(XamlRoot(), winrt::Microsoft::UI::Xaml::ElementTheme::Dark);
    }

    void Welcome::Language_Selected(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto lang{ winrt::unbox_value<hstring>(sender.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Tag()) };
        winrt::Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride(lang);
    }
}
