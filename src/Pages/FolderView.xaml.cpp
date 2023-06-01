#include "pch.h"
#include "FolderView.xaml.h"
#if __has_include("FolderView.g.cpp")
#include "FolderView.g.cpp"
#endif

#include "SettingsHelper.h"
#include "Data.h"
#include <set>

namespace winrt::Player::implementation
{
    FolderView::FolderView()
    {
        InitializeComponent();
        // because constructor cannot be coroutine, so initialize in Loaded event
    }

    winrt::Windows::Foundation::IAsyncAction FolderView::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
    {
        if (args.Parameter() == nullptr)
            co_return;

        auto argument{ args.Parameter().try_as<winrt::Data::FolderViewParameter>() };

        auto library_info{ argument.Library() };
        if (library_ == library_info)
            co_return;
        else
            library_ = library_info;

        player_view_model_ = argument.PlayerViewModel();
        info_list_ = argument.Music();
        play_list_ = argument.List();
        music_list_ = play_list_.Items();

        folders_stack_.clear();
        path_stack_.clear();
        folders_view_.Clear();
        music_view_.Clear();
        whole_library = ::Data::TramsformJsonArrayToVector((co_await SettingsHelper::GetLibrary(library_.name)));
        BuildRoot();

        // init FolderList UI item events
        FolderViewList().SelectionChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
            // don't use ItemClick event because the return type of args.SelectedItem() is unknown
            auto index{ self.FolderViewList().SelectedIndex() };
            if (index == -1)
                return;                              // workaround beacuse one click trigged event twice
            self.FolderViewList().SelectedIndex(-1); // workaround becaues winrt think same string is same object, so if return back, it does't change the selectindex
            auto path{ self.folders_view_.GetAt(index) };
            if (path == L".\u2005.\u2005.\u2005") [[unlikely]]
            {
                // back to previous level
                self.folders_stack_.pop_back();
                self.path_stack_.pop_back();
                if (self.path_stack_.empty()) [[unlikely]]
                {
                    // backed is root
                    self.BuildRoot();
                }
                else
                {
                    // backed not root
                    self.Rebuild();
                }
            }
            else
            {
                // forward to next level
                self.path_stack_.push_back(path);
                self.Build();
            }
        });

        /* This is a confusing workaround :
            ListView only produces click events when using both of click mode
            and single selection mode at the same time, and setting selected item programmatically is invalid,
            but this does not mean that there is no selection effect
            (different from setting the selection mode to None, which does not produce any selection effect),
            so in clicked event handler, cancel the click mode, turn on the single selection mode,
            and then call SelectedItem(item) to trigger the single selection change event.
            This way can achieve both click events and single selection events,
            making it possible to change the ListView UI programmatically without trigger single selection change event
            (because ListView is almost always in click mode,
            so changing selected item programmatically does not trigger single selection events)
        */
        MusicViewList().ItemClick([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args) -> winrt::Windows::Foundation::IAsyncAction {
            // important code to make such effect
            auto view_list{ self.MusicViewList() };
            view_list.SelectionMode(winrt::Microsoft::UI::Xaml::Controls::ListViewSelectionMode::Single);
            view_list.IsItemClickEnabled(false);
            view_list.SelectedItem(args.ClickedItem());

            // find index of clicked item
            auto view{ self.music_view_.GetView() };
            auto current{ args.ClickedItem().try_as<winrt::Player::InfoViewModel>().Get() };
            // swith to backgroud thread
            co_await winrt::resume_background();
            auto index{ -1ll };
            {
                for (auto begin{ view.begin() }, end{ view.end() }; begin != end; ++begin)
                {
                    if (current.Duration == (*begin).Duration())
                    {
                        index = std::distance(view.begin(), begin);
                        break;
                    }
                }
                if (index == -1ll)
                    co_return;
            }
            // prepare music list data
            auto items{ std::vector<winrt::Windows::Media::Playback::MediaPlaybackItem>{} };
            auto infos{ std::vector<winrt::Data::Music>{} };
            {
                auto size{ view.Size() };
                items.reserve(size);
                infos.reserve(size);
                for (auto const& i : self.music_view_.GetView())
                {
                    auto info{ i.Get() };
                    auto const& file{ co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(self.library_.address + info.Path) };
                    items.emplace_back(winrt::Windows::Media::Core::MediaSource::CreateFromStorageFile(file));
                    infos.emplace_back(info);
                }
            }
            // set data
            self.player_view_model_.Library(self.library_);
            self.info_list_.ReplaceAll(infos);
            self.music_list_.ReplaceAll(items);
            self.play_list_.MoveTo(static_cast<uint32_t>(index));
        });
        MusicViewList().SelectionChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args) {
            // make click event effective
            self.MusicViewList().IsItemClickEnabled(true);
        });
        // regist play list event to update selected item
        play_list_.CurrentItemChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(play_list_) const& sender, winrt::Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            auto item{ sender.CurrentItem() };
            if (item == nullptr)
                co_return;
            auto info{ self.info_list_.GetAt(sender.CurrentItemIndex()) };
            co_await winrt::resume_background();
            auto file{ co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(self.library_.address + info.Path) };
            auto thumb{ winrt::Windows::Storage::Streams::RandomAccessStreamReference::CreateFromStream(co_await file.GetThumbnailAsync(winrt::Windows::Storage::FileProperties::ThumbnailMode::MusicView, 400)) };
            auto title{ InfoViewModel::DecisionTitle(info.Title, info.Path) };
            auto artist{ InfoViewModel::DecisionArtist(info.Artist, info.Albumartist) };
            {
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
            {
                co_await ui_thread;
                // update RootPage UI
                self.player_view_model_.Image().SetSource(co_await thumb.OpenReadAsync());
                self.player_view_model_.Album(info.Album);
                self.player_view_model_.Artist(artist);
                self.player_view_model_.Title(title);
                if (artist.empty())
                    self.player_view_model_.AppTitle(title);
                else
                    self.player_view_model_.AppTitle(fast_io::wconcat_winrt_hstring(artist, L" - ", title));
                // update FolderView UI
                for (auto item : self.music_view_)
                {
                    if (info.Duration == item.Duration())
                    {
                        self.MusicViewList().SelectedItem(item);
                        item.SetState(true);
                    }
                    else
                    {
                        item.SetState(false);
                    }
                }
            }
        });
    }

    winrt::hstring FolderView::CalculateTrueFolderCount(uint32_t value)
    {
        auto adjust{ path_stack_.empty() ? 0 : 1 };
        return fast_io::wconcat_winrt_hstring(value - adjust);
    }

    void FolderView::UpdateUI(std::vector<winrt::hstring> const& folders, std::vector<winrt::Player::InfoViewModel> const& music)
    {

        // set info
        {
            folders_view_.ReplaceAll(folders);
            music_view_.ReplaceAll(music);
        }
        // set full_path
        {
            auto full_path{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };
            full_path.resize(full_path.size() + 1uz);
            std::memmove(full_path.data() + 1uz, full_path.data(), (full_path.size() - 1uz) * sizeof(decltype(full_path)::value_type));
            full_path[0] = L'\\';
            FullPath().Text(full_path);
        }
        // set music duration count
        {
            auto count{ winrt::clock::duration{} };
            for (auto const& info : music) [[likely]]
                count += winrt::clock::duration{ info.Duration() };
            auto const hms{ std::chrono::hh_mm_ss{ count } };

            auto text{ winrt::hstring{} };

            if (hms.hours().count()) [[likely]]
                text = fast_io::wconcat_winrt_hstring(hms.hours(), fast_io::manipulators::chvw(L'\u2005'), hms.minutes(), fast_io::manipulators::chvw(L'\u2005'), hms.seconds());
            else if (hms.minutes().count())
                text = fast_io::wconcat_winrt_hstring(hms.minutes(), fast_io::manipulators::chvw(L'\u2005'), hms.seconds());

            DurationCount().Text(text);
        }
        // set level change button
        {
            auto level{ FolderLevel() };
            if (path_stack_.empty()) [[unlikely]]
            {
                level.Content(winrt::box_value(L"\\"));
            }
            else
            {
                level.Content(winrt::box_value(path_stack_.back()));
            }
            auto flyout{ level.Flyout().try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyout>() };
            auto items{ flyout.Items() };
            items.Clear();
            auto event{ [&self = *this](winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&) {
                auto current{ sender.as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Text() };
                if (current[0] == L'\\') [[unlikely]]
                {
                    if (self.path_stack_.empty())
                        return;
                    self.path_stack_.clear();
                    self.folders_stack_.clear();
                    self.BuildRoot();
                }
                else
                {
                    auto count{ 0uz };
                    for (auto const& name : std::views::reverse(self.path_stack_)) [[likely]]
                    {
                        if (name != current) [[likely]]
                            ++count;
                        else
                            break;
                    }
                    self.path_stack_.resize(count);
                    self.folders_stack_.resize(count);
                    self.Rebuild();
                }
                for (auto const& name : self.folders_view_.GetView())
                {
                    if (name == current)
                    {
                        self.FolderViewList().ScrollIntoView(winrt::box_value(name));
                        break;
                    }
                }
            } };
            auto root{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
            root.Text(L"\\");
            root.Tapped(event);
            items.Append(root);
            for (auto const& name : path_stack_) [[likely]]
            {
                auto item{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
                item.Text(name);
                items.Append(item);
                item.Tapped(event);
            }
        }
    }

    void FolderView::BuildRoot()
    {
        // for music_view_
        auto music{ std::vector<winrt::Player::InfoViewModel>{} };
        auto set{ std::set<std::wstring_view>{} };
        {
            music.reserve(20uz);

            // init all data
            for (auto const& info : whole_library) [[likely]]
            {
                auto const path{ std::wstring_view{ info.Path } };
                auto const end{ path.find(L'\\', 2uz) };

                // if is file
                if (end == decltype(path)::npos)
                    music.emplace_back(info);
                // if is folder
                else
                    set.emplace(path.begin() + 1uz, path.begin() + end);
            }
        }
        // for folders_view_
        auto folders{ std::vector<winrt::hstring>{} };
        {
            folders.reserve(20uz);
            for (auto const& i : set) [[likely]]
                folders.emplace_back(i);
        }

        UpdateUI(folders, music);
    }

    void FolderView::Build()
    {

        auto full_path{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };
        // for music_view_
        auto music{ std::vector<winrt::Player::InfoViewModel>{} };
        // for current stack frame
        auto frame{ std::vector<std::reference_wrapper<const winrt::Data::Music>>{} };
        {
            music.reserve(20uz);
            frame.reserve(50uz);

            if (folders_stack_.empty()) [[unlikely]]
            {
                // current is root
                for (auto const& info : whole_library) [[likely]]
                {
                    auto const path{ std::wstring_view{ info.Path } };
                    if (path.find(full_path, 1uz) != decltype(path)::npos)
                        frame.emplace_back(info);
                }
            }
            else
            {
                for (auto const& info : folders_stack_.back()) [[likely]]
                {
                    auto const path{ std::wstring_view{ info.get().Path } };

                    if (path.find(full_path, 1uz) != decltype(path)::npos)
                        frame.emplace_back(info);
                }
            }
        }
        auto set{ std::set<std::wstring_view>{} };
        for (auto const& i : frame) [[likely]]
        {
            auto const& info{ i.get() };
            auto const path{ std::wstring_view{ info.Path }.substr(full_path.size() + 1uz) };
            auto const end{ path.find(L'\\', 2uz) };

            // if is file
            if (end == decltype(path)::npos)
                music.emplace_back(info);
            // if is folder
            else
                set.emplace(path.begin() + 1uz, path.begin() + end);
        }
        // for folders_view_
        auto folders{ std::vector<winrt::hstring>{} };
        {
            folders.reserve(20uz);
            folders.emplace_back(L".\u2005.\u2005.\u2005");
            for (auto const& info : set) [[likely]]
                folders.emplace_back(info);

            folders_stack_.push_back(std::move(frame));
        }

        UpdateUI(folders, music);
    }

    void FolderView::Rebuild()
    {

        // for music_view_
        auto music{ std::vector<winrt::Player::InfoViewModel>{} };
        auto set{ std::set<std::wstring_view>{} };
        {
            music.reserve(20uz);

            auto const& frame{ folders_stack_.back() };
            auto full_path{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };

            for (auto const& i : frame) [[likely]]
            {
                auto const& info{ i.get() };
                auto const path{ std::wstring_view{ info.Path }.substr(full_path.size() + 1uz) };
                auto const end{ path.find(L'\\', 2uz) };

                // if is file
                if (end == decltype(path)::npos)
                    music.emplace_back(info);
                // if is folder
                else
                    set.emplace(path.begin() + 1uz, path.begin() + end);
            }
        }

        // for folders_view_
        auto folders{ std::vector<winrt::hstring>{} };
        {
            folders.reserve(20uz);
            folders.emplace_back(L".\u2005.\u2005.\u2005");
            for (auto const& info : set) [[likely]]
                folders.emplace_back(info);
        }

        UpdateUI(folders, music);
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FolderView::FolderList()
    {
        return folders_view_;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> FolderView::MusicList()
    {
        return music_view_;
    }
}
