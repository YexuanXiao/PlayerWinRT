#include "pch.h"
#include "RootPage.xaml.h"
#if __has_include("RootPage.g.cpp")
#include "RootPage.g.cpp"
#endif

#include "SettingsHelper.h"
#include "Data.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Input;

using namespace Windows::Foundation;

namespace winrt::Player::implementation
{
    RootPage::RootPage() : session_(player_.PlaybackSession()), commander_(player_.CommandManager())
    {
        InitializeComponent();

        // init state
        if (SettingsHelper::CheckFirstUse()) [[unlikely]] {
            RootFrame().Navigate(winrt::xaml_typename<Player::Welcome>());
            MainNavigation().IsPaneOpen(false);
            }
            // prepare list
        {
            list_.MaxPlayedItemsToKeepOpen(3);
            // set player state
            player_.Source(list_);
            player_.AudioCategory(Windows::Media::Playback::MediaPlayerAudioCategory::Media);
#ifdef _DEBUG
            // player_.Source(Windows::Media::Core::MediaSource::CreateFromUri(Uri{ L"ms-appx:///Assets/24 - 英雄のタクト.flac" }));
            music_.VectorChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(music_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) -> IAsyncAction {
                auto operate{ args.CollectionChange() };
                auto index{ args.Index() };
                switch (operate) {
                case winrt::Windows::Foundation::Collections::CollectionChange::ItemRemoved:
                {
                    break;
                }
                case winrt::Windows::Foundation::Collections::CollectionChange::ItemChanged:
                {
                    break;
                }
                case winrt::Windows::Foundation::Collections::CollectionChange::ItemInserted:
                {
                    break;
                }
                case winrt::Windows::Foundation::Collections::CollectionChange::Reset:
                    self.player_.Play();
                    co_await ui_thread;
                    self.TogglePlayButton();
                }
                });
#endif
        }
            // prepare libraries
            {
                auto libraries{ SettingsHelper::GetLibraries() };

                // make container and data
                auto container{ std::vector<winrt::Data::Library>{} };
                container.reserve(libraries.Size());

                for (auto const& library : libraries) {
                    auto info{ library.GetObjectW() };
                    container.emplace_back(info.GetNamedString(L"Name"), info.GetNamedString(L"Protocol"), info.GetNamedString(L"Path"), info.GetNamedString(L"Icon"));
                }
                {
                    // prepare ui elements
                    auto menulist{ MainLibraryList().MenuItems() };
                    auto itemadd{ *menulist.begin() };
                    menulist.Clear();
                    for (auto const& library : container) {
                        menulist.Append(RootPage::MakeNavItem(library));
                    }
                    menulist.Append(itemadd);
                }
                {
                    // move container to observer vector
                    libraries_ = winrt::single_threaded_observable_vector<winrt::Data::Library>(std::move(container));
                    // add event to update menu list ui
                    libraries_.VectorChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(libraries_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) -> IAsyncAction {
                        co_await ui_thread;
                        auto operate{ args.CollectionChange() };
                        auto index{ args.Index() };
                        auto menulist{ self.MainLibraryList().MenuItems() };
                        switch (operate) {
                        case winrt::Windows::Foundation::Collections::CollectionChange::ItemRemoved:
                        {
                            menulist.RemoveAt(index);
                            break;
                        }
                        case winrt::Windows::Foundation::Collections::CollectionChange::ItemChanged:
                        {
                            menulist.SetAt(index, self.MakeNavItem(libraries_.GetAt(index)));
                            break;
                        }
                        case winrt::Windows::Foundation::Collections::CollectionChange::ItemInserted:
                        {
                            menulist.InsertAt(index, self.MakeNavItem(libraries_.GetAt(index)));
                            break;
                        }
                        }
                        });
                }
            }
            // regist volume change
            playerViewModel_.PropertyChanged([&self = *this](IInspectable const&, PropertyChangedEventArgs const& args) {
                if (args.PropertyName() == L"Volume")
                    self.player_.Volume(self.playerViewModel_.Volume() / 100.);
                });
            // regist play and pause event
            commander_.PlayReceived([&self = *this](Windows::Media::Playback::MediaPlaybackCommandManager const&, Windows::Media::Playback::MediaPlaybackCommandManagerPlayReceivedEventArgs const&) {
                self.TogglePlayButton();
                });
            commander_.PauseReceived([&self = *this](Windows::Media::Playback::MediaPlaybackCommandManager const&, Windows::Media::Playback::MediaPlaybackCommandManagerPauseReceivedEventArgs const&) {
                self.TogglePlayButton();
                });
            PlayButton().Tapped([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&) {
                if (self.TogglePlayButton())
                    // if is paused
                    self.player_.Play();
                else if (self.session_.CanPause())
                    self.player_.Pause();
                });
    }
    hstring RootPage::AppTitleText() {
#ifdef _DEBUG
        return L"PlayerWinRT Dev";
#else
        return title_;
#endif
    }
    // return true if is paused, ready to play
    bool RootPage::TogglePlayButton() {
        auto fontIcon{ PlayButton().Content().try_as<FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        {
            // cacl data
            if (icon == L"\uE768") {
                fontIcon.Glyph(L"\uE769");
                fontIcon.Margin(ThicknessHelper::FromUniformLength(0));
                return true;
            }
            else if (icon == L"\uE769") {
                fontIcon.Glyph(L"\uE768");
                fontIcon.Margin(ThicknessHelper::FromLengths(2, 0, 0, 0));
                return false;
            }
            else if (icon == L"\uF5B0") {
                fontIcon.Glyph(L"\uF8AE");
                fontIcon.Margin(ThicknessHelper::FromUniformLength(0));
                return true;
            }
            else {
                // if (icon == L"\uF8AE")
                fontIcon.Glyph(L"\uF5B0");
                fontIcon.Margin(ThicknessHelper::FromLengths(2, 0, 0, 0));
                return false;
            }
        }
    }
    IAsyncAction RootPage::Navigation_ItemInvoked(NavigationView const&, NavigationViewItemInvokedEventArgs const& args) {
        if (args.IsSettingsInvoked()) {
            RootFrame().Navigate(winrt::xaml_typename<Player::Settings>());
        }
        else [[likely]] {
            auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
            auto tagName{ winrt::unbox_value_or<winrt::hstring>(
                args.InvokedItemContainer().Tag(), winrt::hstring{}) };
            auto theme{ ActualTheme() };
            if (tagName == L"about") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(resourceLoader.GetString(L"About/Content")));
                dialog.CloseButtonText(resourceLoader.GetString(L"Close"));
                dialog.DefaultButton(ContentDialogButton::Close);
                auto page{ Player::About{} };
                dialog.Content(page);
                dialog.RequestedTheme(theme);
                static_cast<void>(co_await dialog.ShowAsync());
            }
            else if (tagName == L"add") {
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
            else if (tagName == L"equalizer") {
                auto dialog{ ContentDialog{} };
                dialog.XamlRoot(XamlRoot());
                dialog.Title(winrt::box_value(resourceLoader.GetString(L"Equalizer/Content")));
                dialog.PrimaryButtonText(resourceLoader.GetString(L"Save"));
                dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
                dialog.DefaultButton(ContentDialogButton::Close);
                auto page{ Player::Equalizer{} };
                dialog.Content(page);
                dialog.RequestedTheme(theme);
                auto result{ co_await dialog.ShowAsync() };
                // use return value
            }
            }
    }
    IAsyncAction RootPage::MusicInfo_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto dialog{ ContentDialog{} };
        auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
        dialog.XamlRoot(XamlRoot());
        dialog.Title(winrt::box_value(resourceLoader.GetString(L"MusicInfo/Text")));
        dialog.CloseButtonText(resourceLoader.GetString(L"Close"));
        auto page{ Player::MusicInfo{} };
        dialog.Content(page);
        dialog.RequestedTheme(ActualTheme());
        static_cast<void>(co_await dialog.ShowAsync());
    }
    void RootPage::Repeat_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto fontIcon{ sender.try_as<Button>().Content().try_as<FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        if (icon == L"\uF5E7") {
            icon = L"\uE8ED";
        }
        else if (icon == L"\uE8ED") {
            icon = L"\uE8EE";
        }
        else if (icon == L"\uE8EE") {
            icon = L"\uF5E7";
        }
        fontIcon.Glyph(icon);
    }

    void RootPage::Shuffle_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto fontIcon{ sender.try_as<Button>().Content().try_as<FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        if (icon == L"\uE8B1") {
            icon = L"\uE70F";
        }
        else if (icon == L"\uE70F") {
            icon = L"\uE8B1";
        }
        fontIcon.Glyph(icon);
    }
    Windows::Media::Playback::MediaPlayer RootPage::Player() {
        return player_;
    }
    Windows::Media::Playback::MediaPlaybackList RootPage::List() {
        return list_;
    }
    PlayerViewModel RootPage::PlayerViewModel() {
        return playerViewModel_;
    }
    winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> RootPage::Libraries() {
        return libraries_;
    }
    winrt::Windows::Foundation::Collections::IObservableVector<Windows::Media::Playback::MediaPlaybackItem> RootPage::Music() {
        return music_;
    }
    winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem RootPage::MakeNavItem(const winrt::Data::Library& library) {
        auto item{ NavigationViewItem{} };
        {
            item.Content(winrt::box_value(library.name));
            auto tag{ winrt::box_value(library) };
            item.Tag(tag);
            item.Tapped([&self = *this](winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args) {

                auto item{ sender.try_as<NavigationViewItem>() };
                auto tag{ item.Tag() };
                auto info{ winrt::unbox_value<winrt::Data::Library>(tag) };
                ::Data::Global::CurrentLibrary = { info.name, info.protocol, info.address, info.icon };
                auto current{ self.Current() };
                current.Content(winrt::box_value(info.name));
                current.Icon().try_as<FontIcon>().Glyph(info.icon);
                current.Visibility(Visibility::Visible);
                current.Tag(tag);
                current.IsSelected(true);
                self.MainLibraryList().IsExpanded(self.MainNavigation().IsPaneOpen());
                self.NavigateToDefaultPage();
                args.Handled(true);
                });
            // menu
            {
                auto menuFlyout{ MenuFlyout{} };
                auto up{ MenuFlyoutItem{} };
                auto down{ MenuFlyoutItem{} };
                auto edit{ MenuFlyoutItem{} };
                auto remove{ MenuFlyoutItem{} };
                {
                    auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
                    auto upIcon{ FontIcon{} };
                    auto downIcon{ FontIcon{} };
                    auto editIcon{ FontIcon{} };
                    auto removeIcon{ FontIcon{} };

                    // todo: add actual functionality
                    upIcon.Glyph(L"\uE70E");
                    downIcon.Glyph(L"\uE70D");
                    editIcon.Glyph(L"\uE70F");
                    removeIcon.Glyph(L"\uE74D");
                    up.Text(resourceLoader.GetString(L"Up/Text"));
                    down.Text(resourceLoader.GetString(L"Down/Text"));
                    edit.Text(resourceLoader.GetString(L"Edit/Text"));
                    remove.Text(resourceLoader.GetString(L"Delete/Text"));
                    up.Icon(upIcon);
                    down.Icon(downIcon);
                    edit.Icon(editIcon);
                    remove.Icon(removeIcon);
                    up.Tag(tag);
                    down.Tag(tag);
                    edit.Tag(tag);
                    remove.Tag(tag);
                }
                {
                    auto items{ menuFlyout.Items() };
                    items.Append(up);
                    items.Append(down);
                    items.Append(edit);
                    items.Append(remove);
                }
                item.ContextFlyout(menuFlyout);
            }
            // icon
            auto icon{ FontIcon{} };
            icon.Glyph(library.icon);
            item.Icon(icon);
        }
        return item;
    }

    void RootPage::Current_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto tag{ winrt::unbox_value<winrt::Data::Library>(sender.try_as<NavigationViewItem>().Tag()) };
        if (::Data::Global::CurrentLibrary.name == tag.name) return;
        NavigateToDefaultPage();
    }
    void RootPage::NavigateToDefaultPage() {
        RootFrame().Navigate(winrt::xaml_typename<winrt::Player::FolderView>());
    }
    void RootPage::Navigation_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const&, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&)
    {
        RootFrame().GoBack();
    }

    void RootPage::Page_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        // set theme
        auto theme{ SettingsHelper::LoadTheme() };
        SettingsHelper::SetTheme(XamlRoot(), theme);
    }
    void RootPage::Volume_LostFocus(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        SettingsHelper::SetVolume(playerViewModel_.Volume());
    }
}
