#include "pch.h"
#include "RootPage.xaml.h"
#if __has_include("RootPage.g.cpp")
#include "RootPage.g.cpp"
#endif

#include "SettingsHelper.h"
#include "Data.h"

namespace winrt::Player::implementation
{
    RootPage::RootPage()
    {
        InitializeComponent();

        list_.MaxPlayedItemsToKeepOpen(3);
        player_.AudioCategory(Windows::Media::Playback::MediaPlayerAudioCategory::Media);
        player_.Source(list_);
        {
            // prepare and update ui elements
            auto menulist{ MainLibraryList().MenuItems() };
            auto itemadd{ *menulist.begin() };
            menulist.Clear();
            for (auto const& library : libraries_.GetView()) {
                menulist.Append(RootPage::MakeNavItem(library));
            }
            menulist.Append(itemadd);
        }

        InitializeRegistEvents();

        // init state
        if (SettingsHelper::CheckFirstUse()) [[unlikely]]
        {
            RootFrame().Navigate(winrt::xaml_typename<Player::Welcome>(), libraries_);
            MainNavigation().IsPaneOpen(false);
        }
    }

    void RootPage::InitializeRegistEvents() {
        // this function can be called once at any position in the constructor
        // update libraries ui
        libraries_.VectorChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(libraries_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) -> winrt::Windows::Foundation::IAsyncAction {
            co_await ui_thread;
            auto operate{ args.CollectionChange() };
            auto index{ args.Index() };
            auto menulist{ self.MainLibraryList().MenuItems() };
            switch (operate) {
            case decltype(operate)::ItemRemoved:
            {
                menulist.RemoveAt(index);
                break;
            }
            case decltype(operate)::ItemChanged:
            {
                menulist.SetAt(index, self.MakeNavItem(self.libraries_.GetAt(index)));
                break;
            }
            case decltype(operate)::ItemInserted:
            {
                menulist.InsertAt(index, self.MakeNavItem(self.libraries_.GetAt(index)));
                break;
            }
            }
            });
        // volume change
        playerViewModel_.PropertyChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, PropertyChangedEventArgs const&) {
            self.player_.Volume(self.playerViewModel_.Volume() / 100.);
            });
        // slider
        session_.PlaybackStateChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(session_) const& sender, winrt::Windows::Foundation::IInspectable const&) -> winrt::Windows::Foundation::IAsyncAction {
            auto state{ self.session_.PlaybackState() };
            using namespace std::chrono_literals;
            if (state == decltype(state)::Playing) [[likely]] {
                auto index{ self.list_.CurrentItemIndex() };
                auto info{ self.info_list_.GetAt(index) };
                co_await ui_thread;
                self.playerViewModel_.Duration(static_cast<double>(info.Duration));
            }
            while (self.session_.PlaybackState() == decltype(self.session_.PlaybackState())::Playing) [[likely]] {
                using namespace std::chrono_literals;
                co_await 1s;
                co_await ui_thread;
                auto position{ self.session_.Position().count() };
                if (position > 0) [[likely]] {
                    self.playerViewModel_.Position(static_cast<double>(position));
                    }
                }
            });
        playerViewModel_.PropertyChanged([&self = *this, ui_thread = winrt::apartment_context{}](winrt::Windows::Foundation::IInspectable const&, PropertyChangedEventArgs const& args) {
            // if two events occur within 50ms, discard subsequent events
            {
                static auto pre{ winrt::clock::now().time_since_epoch() };
                auto now{ winrt::clock::now().time_since_epoch() };
                using namespace std::chrono_literals;
                if ((now - pre) < std::chrono::duration_cast<winrt::clock::duration>(50ms))
                    return;
                else
                    pre = now;
            }
            if (args.PropertyName() == L"Position") [[likely]]
                self.session_.Position(std::chrono::duration_cast<decltype(self.session_.Position())>(std::chrono::nanoseconds{ static_cast<int64_t>(self.playerViewModel_.Position()) * 100 }));
            });
        // when switch to new music, make button ui on
        list_.CurrentItemChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(list_) const&, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs const& args) -> winrt::Windows::Foundation::IAsyncAction {
            auto reason{ args.Reason() };
            // repeat one
            if (reason == decltype(reason)::EndOfStream && self.repeat_one_ == true) {
                auto index{ self.list_.CurrentItemIndex() };
                self.list_.MoveTo(index > 0u ? index - 1u : 0u);
                co_return;
            }
            auto item{ self.list_.CurrentItem() };
            if (item == nullptr) co_return;
            auto state{ self.session_.PlaybackState() };
            // check if is new list
            if (state == decltype(state)::Opening || state == decltype(state)::Paused) {
                self.player_.Play();
            }
            co_await winrt::resume_background();
            auto library{ self.playerViewModel_.Library() };
            if (library.protocol == L"local") {
                auto index{ self.list_.CurrentItemIndex() };
                auto info{ self.info_list_.GetAt(index) };
                auto const& file{ winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(library.address + info.Path).get() };
                auto prop{ item.GetDisplayProperties() };
                prop.Type(winrt::Windows::Media::MediaPlaybackType::Music);
                auto thumb{ winrt::Windows::Storage::Streams::RandomAccessStreamReference::CreateFromStream(file.GetThumbnailAsync(winrt::Windows::Storage::FileProperties::ThumbnailMode::MusicView).get()) };
                prop.Thumbnail(thumb);
                auto musicprop{ prop.MusicProperties() };
                prop.Type(winrt::Windows::Media::MediaPlaybackType::Music);
                auto title{ FolderView::DecisionTitle(info.Title, info.Path) };
                musicprop.Title(title);
                musicprop.AlbumArtist(info.Albumartist);
                musicprop.AlbumTitle(info.Album);
                auto artist{ FolderView::DecisionArtist(info.Artist, info.Albumartist) };
                musicprop.Artist(artist);
                musicprop.Genres().Append(info.Genre);
                musicprop.TrackNumber(info.Track);
                item.ApplyDisplayProperties(prop);
                {
                    co_await ui_thread;
                    auto image{ winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage{} };
                    image.SetSource(co_await thumb.OpenReadAsync());
                    self.PlayerPicture().Source(image);
                    self.PlayerArtist().Text(artist);
                    self.PlayerTitle().Text(title);
                }
            }
            co_await ui_thread;
            self.PlayButtonOn();
            });
        // regist play and pause event to update button ui
        commander_.PlayReceived([&self = *this, ui_thread = winrt::apartment_context{}](decltype(commander_), Windows::Media::Playback::MediaPlaybackCommandManagerPlayReceivedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            co_await ui_thread;
            self.PlayButtonOn();
            });
        commander_.PauseReceived([&self = *this, ui_thread = winrt::apartment_context{}](decltype(commander_) const&, Windows::Media::Playback::MediaPlaybackCommandManagerPauseReceivedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            co_await ui_thread;
            self.PlayButtonOff();
            });
        /* listen to music list change
        music_.VectorChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(music_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) {
            auto operate{ args.CollectionChange() };
            switch (operate) {
            case decltype(operate)::ItemRemoved:
            {
                break;
            }
            case decltype(operate)::ItemChanged:
            {
                break;
            }
            case decltype(operate)::ItemInserted:
            {
                break;
            }
            case decltype(operate)::Reset:
                break;
            }
            });
        */
    }
    hstring RootPage::AppTitleText() {
#ifdef _DEBUG
        return L"PlayerWinRT Dev";
#else
        return title_;
#endif
    }
    void RootPage::PlayButtonOn() {
        auto fontIcon{ PlayButton().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        if (icon == L"\uE768") {
            fontIcon.Glyph(L"\uE769");
        }
        else if (icon == L"\uF5B0") {
            fontIcon.Glyph(L"\uF8AE");
        }
        fontIcon.Margin(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromUniformLength(0));
    }
    void RootPage::PlayButtonOff() {
        auto fontIcon{ PlayButton().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        if (icon == L"\uE769") {
            fontIcon.Glyph(L"\uE768");
        }
        else if (icon == L"\uF8AE") {
            fontIcon.Glyph(L"\uF5B0");
        }
        fontIcon.Margin(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromLengths(2, 0, 0, 0));
    }
    void RootPage::Navigation_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const&, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args) {
        if (args.IsSettingsInvoked()) {
            RootFrame().Navigate(winrt::xaml_typename<Player::Settings>());
        }
    }
    winrt::Windows::Foundation::IAsyncAction RootPage::MusicInfo_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);

        auto dialog{ winrt::Player::MusicInfo{} };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }
    void RootPage::Repeat_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto fontIcon{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        if (icon == L"\uF5E7") {
            // repeat all
            icon = L"\uE8EE";
            list_.ShuffleEnabled(false);
            list_.AutoRepeatEnabled(true);
        }
        else if (icon == L"\uE8EE") {
            // repeat one
            repeat_one_ = true;
            icon = L"\uE8ED";
        }
        else if (icon == L"\uE8ED") {
            // no repeat
            repeat_one_ = false;
            icon = L"\uF5E7";
            list_.AutoRepeatEnabled(false);
        }
        fontIcon.Glyph(icon);
    }

    void RootPage::Shuffle_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto fontIcon{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        if (icon == L"\uE8B1") {
            icon = L"\uE70F";
        }
        else if (icon == L"\uE70F") {
            icon = L"\uE8B1";
        }
        fontIcon.Glyph(icon);
    }
    PlayerViewModel RootPage::PlayerViewModel() {
        return playerViewModel_;
    }
    winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem RootPage::MakeNavItem(const winrt::Data::Library& library) {
        auto item{ winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem{} };
        {
            item.Content(winrt::box_value(library.name));
            auto tag{ winrt::box_value(library) };
            item.Tag(tag);
            item.Tapped([&self = *this](winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args) {
                args.Handled(true);
                auto item{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>() };
                auto tag{ item.Tag() };
                auto info{ winrt::unbox_value<winrt::Data::Library>(tag) };
                self.library_ = { info.name, info.protocol, info.address, info.icon };
                auto current{ self.Current() };
                current.Content(winrt::box_value(info.name));
                current.Icon().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>().Glyph(info.icon);
                current.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
                current.Tag(tag);
                self.MainLibraryList().IsExpanded(false);
                self.NavigateToDefaultPage();
                });
            // menu
            {
                auto menuFlyout{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyout{} };
                auto up{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
                auto down{ decltype(up){} };
                auto edit{ decltype(up){} };
                auto remove{ decltype(up){} };
                {
                    auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
                    auto upIcon{ winrt::Microsoft::UI::Xaml::Controls::FontIcon{} };
                    auto downIcon{ decltype(upIcon){} };
                    auto editIcon{ decltype(upIcon){} };
                    auto removeIcon{ decltype(upIcon){} };

                    // todo: add actual function
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
            auto icon{ winrt::Microsoft::UI::Xaml::Controls::FontIcon{} };
            icon.Glyph(library.icon);
            item.Icon(icon);
        }
        return item;
    }

    void RootPage::Current_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto tag{ winrt::unbox_value<winrt::Data::Library>(sender.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>().Tag()) };
        if (library_.name == tag.name) return;
        NavigateToDefaultPage();
    }
    void RootPage::NavigateToDefaultPage() {
        Folders().IsSelected(true);
        RootFrame().Navigate(winrt::xaml_typename<winrt::Player::FolderView>(), winrt::Data::FolderViewParamater{playerViewModel_, info_list_, list_, library_});
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
        // if two events occur within 50ms, discard subsequent events
        {
            static auto pre{ winrt::clock::now().time_since_epoch() };
            auto now{ winrt::clock::now().time_since_epoch() };
            using namespace std::chrono_literals;
            if ((now - pre) < std::chrono::duration_cast<winrt::clock::duration>(50ms))
                return;
            else
                pre = now;
        }
        SettingsHelper::SetVolume(playerViewModel_.Volume());
    }
    void RootPage::PlayButton_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        if (session_.PlaybackState() != winrt::Windows::Media::Playback::MediaPlaybackState::Paused && session_.CanPause()) {
            PlayButtonOff();
            player_.Pause();
        }
        else {
            PlayButtonOn();
            player_.Play();
        }
    }
    Windows::Foundation::IAsyncAction RootPage::Add_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);

        auto dialog{ winrt::Player::LibraryEditor{libraries_} };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }
    Windows::Foundation::IAsyncAction RootPage::About_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);

        auto dialog{ winrt::Player::About{} };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }
    Windows::Foundation::IAsyncAction RootPage::Equalizer_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);

        auto dialog{ winrt::Player::Equalizer{} };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }

    void RootPage::Folders_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        RootFrame().Navigate(winrt::xaml_typename<winrt::Player::FolderView>(), winrt::Data::FolderViewParamater{ playerViewModel_, info_list_, list_, library_ });
    }

    void RootPage::Previous_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto size{ music_.Size() };
        if (size == 0u) return;
        if (list_.CurrentItemIndex() != 0u)
            list_.MovePrevious();
        else
            list_.MoveTo(size);
        if (session_.PlaybackState() == winrt::Windows::Media::Playback::MediaPlaybackState::Paused)
            player_.Play();
    }
    void RootPage::Next_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto size{ music_.Size() };
        if (size == 0) return;
        if (list_.CurrentItemIndex() != size)
            list_.MoveNext();
        else
            list_.MoveTo(0u);
        if (session_.PlaybackState() == winrt::Windows::Media::Playback::MediaPlaybackState::Paused)
            player_.Play();
    }
}
