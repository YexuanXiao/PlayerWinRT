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
    RootPage::RootPage()
    {
        InitializeComponent();

        {
            player_.Source(list_);
            player_.AudioCategory(Windows::Media::Playback::MediaPlayerAudioCategory::Media);
            list_.MaxPlayedItemsToKeepOpen(3);

            // regist volume change
            playerViewModel_.PropertyChanged({ this,&RootPage::UpdateVolume });
        }
        // init state
        if (SettingsHelper::CheckFirstUse()) {
            RootFrame().Navigate(winrt::xaml_typename<Player::Welcome>());
            MainNavigation().IsPaneOpen(false);
#ifndef _DEBUG
            return;
#endif // !_DEBUG
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
            // prepare ui elements
            auto menulist{ MainLibraryList().MenuItems() };
            {
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
                libraries_.VectorChanged([this, menulist, ui_thread = winrt::apartment_context{}](decltype(libraries_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) -> IAsyncAction {
#pragma warning(disable: 26810)
                    co_await ui_thread;
#pragma warning(default: 26810)
                    auto operate{ args.CollectionChange() };
                    auto index{ args.Index() };
                    switch (operate) {
                    case winrt::Windows::Foundation::Collections::CollectionChange::ItemRemoved:
                    {
                        menulist.RemoveAt(index);
                        break;
                    }
                    case winrt::Windows::Foundation::Collections::CollectionChange::ItemChanged:
                    {
                        menulist.SetAt(index, RootPage::MakeNavItem(libraries_.GetAt(index)));
                        break;
                    }
                    case winrt::Windows::Foundation::Collections::CollectionChange::ItemInserted:
                    {
                        menulist.InsertAt(index, RootPage::MakeNavItem(libraries_.GetAt(index)));
                        break;
                    }
                    }
                    });
            }
        }
        // prepare list
        list_.Items().Append(
            Windows::Media::Playback::MediaPlaybackItem{
            Windows::Media::Core::MediaSource::CreateFromUri(
                Uri{ L"ms-appx://Assets/24 - 英雄のタクト.flac" }
            ) }
        );
    }
    hstring RootPage::AppTitleText() {
#ifdef _DEBUG
        return L"PlayerWinRT Dev";
#else
        return L"PlayerWinRT";
#endif
    }
    void RootPage::AppTitleText(hstring const&) {
    }
    void RootPage::Navigation_Loaded(IInspectable const&, RoutedEventArgs const&) {
        // you can also add items in code behind
        auto theme{ SettingsHelper::LoadTheme() };
        SettingsHelper::SetTheme(XamlRoot(), theme);
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
    /// <summary>
    ///  RootPage::GetAppTitleBar has two functions: 1. To be compatible with Windows 10,
    /// it needs to get the title bar element for Window::SetTitleBar;
    /// 2. It calculates the draggable area through the AppTitleBar element itself.
    /// If no need to compable with Windows 10, this function can be changed to GetDragRectangles,
    /// directly obtains the draggable area.
    /// </summary>

    void RootPage::On_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        player_.Source(Windows::Media::Core::MediaSource::CreateFromUri(Uri{ L"ms-appx:///Assets/24 - 英雄のタクト.flac" }));
    }

    void RootPage::PlayButton_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        enum ChangedState {
            Play, Pause
        };
        auto state{ Pause };
        auto fontIcon{ sender.try_as<Button>().Content().try_as<FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        {
            // cacl data
            if (icon == L"\uE768") {
                icon = L"\uE769";
                state = Play;
            }
            else if (icon == L"\uE769") {
                icon = L"\uE768";
                state = Pause;
            }
            else if (icon == L"\uF5B0") {
                icon = L"\uF8AE";
                state = Play;
            }
            else if (icon == L"\uF8AE") {
                icon = L"\uF5B0";
                state = Pause;
            }
        }
        {
            // change player state and ui
            if (state == Play) {
                player_.Play();
                fontIcon.Glyph(icon);
                fontIcon.Margin(ThicknessHelper::FromUniformLength(0));
            }
            else if (player_.CanPause()) {
                player_.Pause();
                fontIcon.Glyph(icon);
                fontIcon.Margin(ThicknessHelper::FromLengths(2, 0, 0, 0));
            }
        }
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
    PlayerViewModel RootPage::PlayerMainViewModel() {
        return playerViewModel_;
    }
    void RootPage::UpdateVolume(IInspectable const&, PropertyChangedEventArgs const& e) {
        if (e.PropertyName() == L"Volume") {
            player_.Volume(playerViewModel_.Volume() / 100.);
        }
    }
    winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> RootPage::Libraries() {
        return libraries_;
    }
    winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::MusicInfo> RootPage::Music() {
        return music_;
    }
    winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem RootPage::MakeNavItem(const winrt::Data::Library& library) {
        auto item{ NavigationViewItem{} };
        {
            item.Content(winrt::box_value(library.name));
            auto tag{ winrt::box_value(library) };
            item.Tag(tag);
            item.Tapped([this](winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args) {
                auto item{ sender.try_as<NavigationViewItem>() };
                auto tag{ item.Tag() };
                auto info{ winrt::unbox_value<winrt::Data::Library>(tag) };
                ::Data::Global::CurrentLibrary = { info.name, info.protocol, info.address, info.icon };
                auto current{ this->Current() };
                current.Content(winrt::box_value(info.name));
                current.Icon().try_as<FontIcon>().Glyph(info.icon);
                current.Visibility(Visibility::Visible);
                current.Tag(tag);
                current.IsSelected(true);
                this->RootFrame().Navigate(winrt::xaml_typename<winrt::Player::FolderView>());
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

                    // todo: add true functional
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
        this->RootFrame().Navigate(winrt::xaml_typename<winrt::Player::FolderView>());
    }

    void RootPage::Navigation_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args)
    {
        RootFrame().GoBack();
    }
}