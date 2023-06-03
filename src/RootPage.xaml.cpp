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

        play_list_.MaxPlayedItemsToKeepOpen(3);
        player_.AudioCategory(winrt::Windows::Media::Playback::MediaPlayerAudioCategory::Media);
        player_.Source(play_list_);
        {
            // prepare and update ui elements
            auto menu_list{ MainLibraryList().MenuItems() };
            auto item_add{ *menu_list.begin() };
            menu_list.Clear();
            for (auto const& library : libraries_.GetView())
                menu_list.Append(RootPage::MakeNavItem(library));
            menu_list.Append(item_add);
        }

        InitializeRegistEvents();

        // init state
        if (SettingsHelper::CheckFirstUse()) [[unlikely]]
        {
            RootFrame().Navigate(winrt::xaml_typename<Player::Welcome>(), libraries_);
            MainNavigation().IsPaneOpen(false);
        }
    }

    void RootPage::InitializeRegistEvents()
    {
        // this function can be called once at any position in the constructor
        // update libraries ui
        libraries_.VectorChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(libraries_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) -> winrt::Windows::Foundation::IAsyncAction {
            co_await ui_thread;
            auto operate{ args.CollectionChange() };
            auto index{ args.Index() };
            auto menu_list{ self.MainLibraryList().MenuItems() };
            switch (operate)
            {
            case decltype(operate)::ItemRemoved: {
                menu_list.RemoveAt(index);
                break;
            }
            case decltype(operate)::ItemChanged: {
                menu_list.SetAt(index, self.MakeNavItem(self.libraries_.GetAt(index)));
                break;
            }
            case decltype(operate)::ItemInserted: {
                menu_list.InsertAt(index, self.MakeNavItem(self.libraries_.GetAt(index)));
                break;
            }
            }
        });
        /*
        // hide PlayPicture when navigate to NowPlaying
        RootFrame().Navigated([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args) {
            if (args.SourcePageType().Name == winrt::xaml_typename<winrt::Player::NowPlaying>().Name)
                self.PlayerPicture().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
            else
                self.PlayerPicture().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
        });
        */
        player_view_model_.PropertyChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
            // volume change
            if (args.PropertyName() == L"Volume")
                self.player_.Volume(self.player_view_model_.Volume() / 100.);
            // update Now Playing UI
            if (args.PropertyName() != L"Title")
                return;
            auto title_ui{ self.Title() };
            if (self.player_view_model_.Artist().empty())
            {
                winrt::Microsoft::UI::Xaml::Controls::Grid::SetRowSpan(title_ui, 2);
            }
            else
            {
                winrt::Microsoft::UI::Xaml::Controls::Grid::SetRowSpan(title_ui, 1);
            }
        });
        // slider
        session_.PlaybackStateChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(session_) const&, winrt::Windows::Foundation::IInspectable const&) -> winrt::Windows::Foundation::IAsyncAction {
            while (self.session_.PlaybackState() == decltype(self.session_.PlaybackState())::Playing) [[likely]]
            {
                auto position{ self.session_.Position().count() };
                if (position > 0) [[likely]]
                {
                    co_await ui_thread;
                    self.player_view_model_.Position(static_cast<double>(position));
                    using namespace std::chrono_literals;
                    co_await 1s;
                }
            }
        });
        player_view_model_.PropertyChanged([&self = *this, ui_thread = winrt::apartment_context{}](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
            // if two events occur within 50ms, discard subsequent events
            {
                static auto previous{ winrt::clock::now().time_since_epoch() };
                auto now{ winrt::clock::now().time_since_epoch() };
                using namespace std::chrono_literals;
                if ((now - previous) > std::chrono::duration_cast<winrt::clock::duration>(50ms))
                    previous = now;
                else
                    return;
            }
            if (args.PropertyName() == L"Position") [[likely]]
                self.session_.Position(std::chrono::duration_cast<decltype(self.session_.Position())>(winrt::clock::duration{ static_cast<int64_t>(self.player_view_model_.Position()) }));
        });
        // when switch to new music, make button ui on
        play_list_.CurrentItemChanged([&self = *this, ui_thread = winrt::apartment_context{}, strong = this->get_strong()](decltype(play_list_) const&, winrt::Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs const& args) -> winrt::Windows::Foundation::IAsyncAction {
            auto item{ self.play_list_.CurrentItem() };
            if (item == nullptr)
                co_return;
            auto index{ self.play_list_.CurrentItemIndex() };
            if (args.Reason() == decltype(args.Reason())::EndOfStream && self.repeat_one_ == true)
            {
                // repeat one
                self.play_list_.MoveTo(index > 0u ? index - 1u : 0u);
                co_return;
            }
            co_await ui_thread;
            auto info{ self.info_list_.GetAt(index) };
            // update duration
            self.player_view_model_.Duration(static_cast<double>(info.Duration));
            // check if is new list
            auto state{ self.session_.PlaybackState() };
            if (state == decltype(state)::Opening || state == decltype(state)::Paused) [[unlikely]]
                self.player_.Play();
            self.PlayButtonOn();
            if (self.player_view_model_.Library().protocol == L"local")
            {

                auto file{ co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(self.player_view_model_.Library().address + info.Path) };
                auto thumb{ winrt::Windows::Storage::Streams::RandomAccessStreamReference::CreateFromStream(co_await file.GetThumbnailAsync(winrt::Windows::Storage::FileProperties::ThumbnailMode::MusicView, 400)) };
                auto title{ InfoViewModel::DecisionTitle(info.Title, info.Path) };
                auto artist{ InfoViewModel::DecisionArtist(info.Artist, info.Albumartist) };
                {
                    co_await ui_thread;
                    // update RootPage UI
                    auto image{ winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage{} };
                    co_await image.SetSourceAsync(co_await thumb.OpenReadAsync());
                    self.player_view_model_.Image(image);
                    self.player_view_model_.Album(info.Album);
                    self.player_view_model_.Artist(artist);
                    self.player_view_model_.Title(title);
                }
                {
                    co_await winrt::resume_background();
                    // update SMTC UI
                    auto prop{ item.GetDisplayProperties() };
                    prop.Type(winrt::Windows::Media::MediaPlaybackType::Music);
                    prop.Thumbnail(thumb);
                    prop.Type(winrt::Windows::Media::MediaPlaybackType::Music);
                    auto music_prop{ prop.MusicProperties() };
                    music_prop.Title(title);
                    music_prop.AlbumArtist(info.Albumartist);
                    music_prop.AlbumTitle(info.Album);
                    music_prop.Artist(artist);
                    music_prop.Genres().Append(info.Genre);
                    music_prop.TrackNumber(info.Track);
                    item.ApplyDisplayProperties(prop);
                }
            }
        });
        // regist play and pause event to update button ui
        commander_.PlayReceived([&self = *this, ui_thread = winrt::apartment_context{}](decltype(commander_), winrt::Windows::Media::Playback::MediaPlaybackCommandManagerPlayReceivedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            co_await ui_thread;
            self.PlayButtonOn();
        });
        commander_.PauseReceived([&self = *this, ui_thread = winrt::apartment_context{}](decltype(commander_) const&, winrt::Windows::Media::Playback::MediaPlaybackCommandManagerPauseReceivedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            co_await ui_thread;
            self.PlayButtonOff();
        });
        player_.MediaEnded([&self = *this, ui_thread = winrt::apartment_context{}](decltype(player_) const&, winrt::Windows::Foundation::IInspectable const&) -> winrt::Windows::Foundation::IAsyncAction {
            co_await ui_thread;
            self.PlayButtonOff();
        });
        /* listen to music list change
        music_list_.VectorChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(music_list_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) {
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

    void RootPage::PlayButtonOn()
    {
        auto font_icon{ PlayIcon() };
        auto icon{ font_icon.Glyph() };

        if (icon == L"\uE768")
            font_icon.Glyph(L"\uE769");
        else if (icon == L"\uF5B0")
            font_icon.Glyph(L"\uF8AE");

        font_icon.Margin(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromUniformLength(0));
        auto artist{ player_view_model_.Artist() };
        if (artist.empty())
            player_view_model_.AppTitle(player_view_model_.Title());
        else
            player_view_model_.AppTitle(fast_io::wconcat_winrt_hstring(artist, L" - ", player_view_model_.Title()));
    }

    void RootPage::PlayButtonOff()
    {
        auto font_icon{ PlayIcon() };
        auto icon{ font_icon.Glyph() };

        if (icon == L"\uE769")
            font_icon.Glyph(L"\uE768");
        else if (icon == L"\uF8AE")
            font_icon.Glyph(L"\uF5B0");

        font_icon.Margin(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromLengths(2, 0, 0, 0));
        player_view_model_.AppTitle(L"PlayerWinRT");
    }

    void RootPage::Navigation_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const&, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
    {
        if (args.IsSettingsInvoked())
            RootFrame().Navigate(winrt::xaml_typename<Player::Settings>());
    }

    winrt::Windows::Foundation::IAsyncAction RootPage::MusicInfo_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto index{ play_list_.CurrentItemIndex() };
        if (index == std::numeric_limits<decltype(index)>::max()) [[unlikely]]
            co_return;

        auto dialog{ winrt::Player::MusicInfo{ winrt::Data::MusicInfoParameter{ player_view_model_.Library(), info_list_.GetAt(index) } } };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }

    void RootPage::Repeat_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto fontIcon{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>() };
        auto icon{ fontIcon.Glyph() };
        if (icon == L"\uF5E7")
        {
            // repeat all
            icon = L"\uE8EE";
            play_list_.AutoRepeatEnabled(true);
        }
        else if (icon == L"\uE8EE")
        {
            // repeat one
            repeat_one_ = true;
            icon = L"\uE8ED";
        }
        else if (icon == L"\uE8ED")
        {
            // no repeat
            repeat_one_ = false;
            icon = L"\uF5E7";
            play_list_.AutoRepeatEnabled(false);
        }
        fontIcon.Glyph(icon);
    }

    void RootPage::Shuffle_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto font_icon{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>() };
        auto icon{ font_icon.Glyph() };
        if (icon == L"\uE30D")
        {
            icon = L"\uE8B1";
            play_list_.ShuffleEnabled(true);
        }
        else if (icon == L"\uE8B1")
        {
            play_list_.ShuffleEnabled(false);
            icon = L"\uE30D";
        }
        font_icon.Glyph(icon);
    }

    PlayerViewModel RootPage::PlayerViewModel()
    {
        return player_view_model_;
    }

    winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem RootPage::MakeNavItem(const winrt::Data::Library& library)
    {
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

            auto font{ winrt::Microsoft::UI::Xaml::Application::Current().Resources().Lookup(winrt::box_value(L"IconFontFamily")).try_as<winrt::Microsoft::UI::Xaml::Media::FontFamily>() };
            // menu
            {
                auto menu_flyout{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyout{} };
                auto up{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
                auto down{ decltype(up){} };
                auto edit{ decltype(up){} };
                auto remove{ decltype(up){} };
                {
                    auto resource{ winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
                    auto up_icon{ winrt::Microsoft::UI::Xaml::Controls::FontIcon{} };
                    auto down_icon{ decltype(up_icon){} };
                    auto edit_icon{ decltype(up_icon){} };
                    auto remove_icon{ decltype(up_icon){} };

                    // todo: add actual function
                    up_icon.Glyph(L"\uE70E");
                    down_icon.Glyph(L"\uE70D");
                    edit_icon.Glyph(L"\uE70F");
                    remove_icon.Glyph(L"\uE74D");
                    up_icon.FontFamily(font);
                    down_icon.FontFamily(font);
                    edit_icon.FontFamily(font);
                    remove_icon.FontFamily(font);
                    up.Text(resource.GetString(L"Up/Text"));
                    down.Text(resource.GetString(L"Down/Text"));
                    edit.Text(resource.GetString(L"Edit/Text"));
                    remove.Text(resource.GetString(L"Delete/Text"));
                    up.Icon(up_icon);
                    down.Icon(down_icon);
                    edit.Icon(edit_icon);
                    remove.Icon(remove_icon);
                    up.Tag(tag);
                    down.Tag(tag);
                    edit.Tag(tag);
                    remove.Tag(tag);
                }
                {
                    auto items{ menu_flyout.Items() };
                    items.Append(up);
                    items.Append(down);
                    items.Append(edit);
                    items.Append(remove);
                }
                item.ContextFlyout(menu_flyout);
            }
            // icon
            auto icon{ winrt::Microsoft::UI::Xaml::Controls::FontIcon{} };
            icon.Glyph(library.icon);
            icon.FontFamily(font);
            item.Icon(icon);
        }
        return item;
    }

    void RootPage::Current_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto tag{ winrt::unbox_value<winrt::Data::Library>(sender.try_as<winrt::Microsoft::UI::Xaml::Controls::NavigationViewItem>().Tag()) };
        if (library_.name == tag.name)
            return;
        NavigateToDefaultPage();
    }

    void RootPage::NavigateToDefaultPage()
    {
        Folders().IsSelected(true);
        RootFrame().Navigate(winrt::xaml_typename<winrt::Player::FolderView>(), winrt::Data::ControlPageParameter{ player_view_model_, info_list_, play_list_, library_ });
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
            static auto previous{ winrt::clock::now().time_since_epoch() };
            auto now{ winrt::clock::now().time_since_epoch() };
            using namespace std::chrono_literals;
            if ((now - previous) < std::chrono::duration_cast<winrt::clock::duration>(50ms))
                return;
            else
                previous = now;
        }
        SettingsHelper::SetVolume(player_view_model_.Volume());
    }

    void RootPage::PlayButton_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        if (session_.PlaybackState() != winrt::Windows::Media::Playback::MediaPlaybackState::Paused && session_.CanPause())
        {
            PlayButtonOff();
            player_.Pause();
        }
        else
        {
            PlayButtonOn();
            player_.Play();
        }
    }

    winrt::Windows::Foundation::IAsyncAction RootPage::Add_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);

        auto dialog{ winrt::Player::LibraryEditor{ libraries_ } };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }

    winrt::Windows::Foundation::IAsyncAction RootPage::About_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);

        auto dialog{ winrt::Player::About{} };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }

    winrt::Windows::Foundation::IAsyncAction RootPage::Equalizer_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);

        auto dialog{ winrt::Player::Equalizer{} };
        dialog.XamlRoot(XamlRoot());
        dialog.RequestedTheme(ActualTheme());

        static_cast<void>(co_await dialog.ShowAsync());
    }

    void RootPage::Folders_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        RootFrame().Navigate(winrt::xaml_typename<winrt::Player::FolderView>(), winrt::Data::ControlPageParameter{ player_view_model_, info_list_, play_list_, library_ });
    }

    void RootPage::Previous_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto state{ play_list_.AutoRepeatEnabled() };
        play_list_.AutoRepeatEnabled(true);
        play_list_.MovePrevious();
        play_list_.AutoRepeatEnabled(state);
        if (session_.PlaybackState() == winrt::Windows::Media::Playback::MediaPlaybackState::Paused) [[unlikely]]
            player_.Play();
    }

    void RootPage::Next_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto state{ play_list_.AutoRepeatEnabled() };
        play_list_.AutoRepeatEnabled(true);
        play_list_.MoveNext();
        play_list_.AutoRepeatEnabled(state);
        if (session_.PlaybackState() == winrt::Windows::Media::Playback::MediaPlaybackState::Paused) [[unlikely]]
            player_.Play();
    }

    void RootPage::Mute_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto font_icon{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>().Content().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>() };
        auto icon{ font_icon.Glyph() };
        if (icon == L"\uE767")
        {
            icon = L"\uE74F";
            player_.IsMuted(true);
        }
        else if (icon == L"\uE74F")
        {
            icon = L"\uE767";
            player_.IsMuted(false);
        }
        font_icon.Glyph(icon);
    }

    void RootPage::Page_ActualThemeChanged(winrt::Microsoft::UI::Xaml::FrameworkElement const&, winrt::Windows::Foundation::IInspectable const&)
    {
        auto play{ PlayIcon() };
        auto glyph{ play.Glyph() };
        if (glyph == L"\uE768")
            play.Glyph(L"\uF5B0");
        else
            play.Glyph(L"\uE768");

        auto previous{ PreviousIcon() };
        if (previous.Glyph() == L"\uE892")
            previous.Glyph(L"\uF8AC");
        else
            previous.Glyph(L"\uE892");

        auto next{ NextIcon() };
        if (next.Glyph() == L"\uE893")
            next.Glyph(L"\uF8AD");
        else
            next.Glyph(L"\uE893");
    }

    void RootPage::Playing_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        if (!info_list_.Size())
            return;
        RootFrame().Navigate(winrt::xaml_typename<winrt::Player::NowPlaying>(), winrt::Data::ControlPageParameter{ player_view_model_, info_list_, play_list_, player_view_model_.Library() });
    }
}
