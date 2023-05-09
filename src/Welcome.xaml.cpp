#include "pch.h"
#include "Welcome.xaml.h"
#if __has_include("Welcome.g.cpp")
#include "Welcome.g.cpp"
#endif

#include <winrt/Windows.UI.Core.h>

#include "Win32Helper.h"
#include "SettingsHelper.h"
#include "Data.h"
#include "RootPage.xaml.h"

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
    IAsyncAction Welcome::AddLibrary_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        // show edit library dialog
        auto dialog{ ContentDialog{} };
        auto page{ Player::EditLibrary{nullptr} };
        auto const resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };

        // prepare page
        if (winrt::unbox_value<winrt::hstring>(sender.as<Button>().Tag()) == L"music") {
            auto musicFolder{ Windows::Storage::KnownFolders::MusicLibrary() };
            page = Player::EditLibrary{ musicFolder.DisplayName(),resourceLoader.GetString(L"Local/Text"), Win32Helper::GetMusicFolderPath(), L"\uE770" };
            dialog.Content(page);
        }
        else {
            page = Player::EditLibrary{};
        }

        // set dialog style

        dialog.Content(page);
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());
        dialog.Title(winrt::box_value(resourceLoader.GetString(L"AddLibrary/Content")));
        dialog.PrimaryButtonText(resourceLoader.GetString(L"Add"));
        dialog.SecondaryButtonText(resourceLoader.GetString(L"Cancel"));
        dialog.DefaultButton(ContentDialogButton::Secondary);

        auto json{ winrt::Windows::Data::Json::JsonObject{nullptr} };
        
        // regist add event
        dialog.PrimaryButtonClick([&page, &json](ContentDialog sender, ContentDialogButtonClickEventArgs args) -> IAsyncAction {
            // Cancel = true keep Dialog show after click
            args.Cancel(true);
            sender.Content(Player::Progress{});
            sender.PrimaryButtonText(winrt::hstring{});

            auto const& info{ page.GetResult() };

            if (info.protocol == L"local") {
                auto library{ ::Data::GetLibraryFromFolderPath(info.name, info.protocol, info.address, info.icon) };
                // regist cancel event, must capture by value, otherwise hold a dangling reference
                sender.SecondaryButtonClick([library](ContentDialog const&, ContentDialogButtonClickEventArgs const&) {
                    library.Cancel();
                    });
                // capture calling context.
                auto ui_thread{ winrt::apartment_context{} };
                // switch to other thread
                co_await winrt::resume_background();
                json = co_await library;
                // switch back to ui thread
                co_await ui_thread;
            }
            else {
                // todo: other protocol
            }
            
            // hide dialog
            sender.Hide();
            });
        static_cast<void>(co_await dialog.ShowAsync());

        if (json == nullptr) co_return;

        // consume json
        SettingsHelper::StoreLibrary(json);
        auto info{ winrt::Data::Library{} };
        info.address = json.GetNamedString(L"Path");
        info.icon = json.GetNamedString(L"Icon");
        info.protocol = json.GetNamedString(L"Protocol");
        info.name = json.GetNamedString(L"Name");
        auto libraries{ RootPage::Libraries() };
        libraries.InsertAt(libraries.Size(), info);
    }

    void Welcome::Theme_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SettingsHelper::SetTheme(XamlRoot(), ElementTheme::Dark);
    }

}