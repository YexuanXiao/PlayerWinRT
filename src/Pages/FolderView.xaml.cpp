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
    winrt::Windows::Foundation::IAsyncAction FolderView::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args) {
        if (args.Parameter() == nullptr)
            co_return;
        
        auto argument{ args.Parameter().try_as<winrt::Data::FolderViewParamater>() };
        
        auto library_info{ argument.Library() };
        if (library_info_ == library_info)
            co_return;
        else
            library_info_ = library_info;

        playerViewModel_ = argument.PlayerViewModel();
        music_ = argument.Music();
        list_ = argument.List();
        music_list_ = list_.Items();

        // init FolderList UI item events
        FolderViewList().SelectionChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
            // don't use ItemClick event because the return type of args.SelectedItem() is unknown
            auto index{ self.FolderViewList().SelectedIndex() };
            if (index == -1) return; // workaround beacuse one click trigged event twice
            self.FolderViewList().SelectedIndex(-1); //workaround becaues winrt think same string is same object, so if return back, it does't change the selectindex 
            auto path{ self.folders_view_.GetAt(index) };
            if (path == L".\u2005.\u2005.\u2005") [[unlikely]] {
                // back to previous level
                self.folders_stack_.pop_back();
                self.path_stack_.pop_back();
                if (self.path_stack_.empty()) [[unlikely]] {
                    // backed is root
                    self.BuildRoot();
                    }
                else {
                    // backed not root
                    self.Rebuild();
                }
                }
            else {
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
            auto viewlist{ self.MusicViewList() };
            viewlist.SelectionMode(winrt::Microsoft::UI::Xaml::Controls::ListViewSelectionMode::Single);
            viewlist.IsItemClickEnabled(false);
            viewlist.SelectedItem(args.ClickedItem());

            auto current{ args.ClickedItem().try_as<winrt::Player::InfoViewModel>().Get() };
            // swith to backgroud thread
            co_await winrt::resume_background();
            // find index of clicked item
            auto view{ self.music_view_.GetView() };
            auto index{ -1ll };
            {
                for (auto begin{ view.begin() }, end{ view.end() }; begin != end; ++begin) {
                    if (current.Duration == (*begin).Duration()) {
                        index = std::distance(view.begin(), begin);
                        break;
                    }
                }
                if (index == -1ll) co_return;
            }
            // prepare music list data
            auto itemContainer{ std::vector<winrt::Windows::Media::Playback::MediaPlaybackItem>{} };
            auto infoContainer{ std::vector<winrt::Data::Music>{} };
            {
                auto size{ view.Size() };
                itemContainer.reserve(size);
                infoContainer.reserve(size);
                for (auto const& i : self.music_view_.GetView()) {
                    auto info{ i.Get() };
                    auto const& file{ co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(self.library_info_.address + info.Path) };
                    itemContainer.emplace_back(winrt::Windows::Media::Core::MediaSource::CreateFromStorageFile(file));
                    infoContainer.emplace_back(info);
                }
            }
            // set data
            self.playerViewModel_.Library(self.library_info_);
            self.music_.ReplaceAll(infoContainer);
            self.music_list_.ReplaceAll(itemContainer);
            self.list_.MoveTo(static_cast<uint32_t>(index));
            });
        MusicViewList().SelectionChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
            // make click event effective
            self.MusicViewList().IsItemClickEnabled(true);
            });

        // regist play list event to update selected item
        list_.CurrentItemChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(list_) const& sender, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            if (sender.CurrentItem() == nullptr) co_return;
            co_await ui_thread;
            auto current{ self.music_.GetAt(sender.CurrentItemIndex()) };
            for (const auto& info : self.music_view_.GetView()) {
                if (current.Duration == info.Duration()) {
                    self.MusicViewList().SelectedItem(info);
                    break;
                }
            }
            });

        folders_stack_.clear();
        path_stack_.clear();
        folders_view_.Clear();
        music_view_.Clear();
        library_ = ::Data::TramsformJsonArrayToVector((co_await SettingsHelper::GetLibaray(library_info_.name)));
        BuildRoot();
    }
    winrt::hstring FolderView::CalculateTrueFolderCount(uint32_t value) {
        auto adjust{ path_stack_.empty() ? 0 : 1 };
        return fast_io::wconcat_winrt_hstring(value - adjust);
    }
    winrt::hstring FolderView::TransformBitrate(uint32_t value) {
        return fast_io::wconcat_winrt_hstring(value / 1000, L"kbps");
    }
    winrt::hstring FolderView::TransformDuration(int64_t value) {
        auto const hms{ std::chrono::hh_mm_ss{std::chrono::nanoseconds{static_cast<int64_t>(value) * 100}} };
        auto hours{ hms.hours().count() };
        if (hours)
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hours, 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
        else
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
    }
    winrt::hstring FolderView::DecisionTitle(winrt::hstring const& title, winrt::hstring const& path) {
        if (title.empty()) [[unlikely]] {
            auto pathsv{ std::wstring_view{path} };
            auto end{ pathsv.find_last_of(L'.') };
            auto begin{ pathsv.find_last_of(L'\\') };
            return winrt::hstring{ pathsv.substr(begin + 1uz, end - 1uz) };
            }
        return title;
    }
    const winrt::hstring& FolderView::DecisionArtist(winrt::hstring const& artist, winrt::hstring const& albumartist) {
        if (artist.empty()) [[unlikely]] return albumartist;
        return artist;
    }
    void FolderView::UpdateUI(std::vector<winrt::hstring> const& folders, std::vector<winrt::Player::InfoViewModel> const& music) {

        // set info
        {
            folders_view_.ReplaceAll(folders);
            music_view_.ReplaceAll(music);
        }
        // set fullpath
        {
            auto fullpath{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };
            fullpath.resize(fullpath.size() + 1uz);
            std::memmove(fullpath.data() + 1uz, fullpath.data(), (fullpath.size() - 1uz) * sizeof(decltype(fullpath)::value_type));
            fullpath[0] = L'\\';
            FullPath().Text(fullpath);
        }
        // set music duration count
        {
            auto time{ 0ll };
            for (auto const& info : music) [[likely]]
                time += info.Duration();
            auto const hms{ std::chrono::hh_mm_ss{std::chrono::nanoseconds{time * 100}} };
            auto text{ std::wstring{} };
            text.reserve(20uz);

            if (hms.hours().count()) [[likely]] {
                text += fast_io::wconcat(hms.hours().count());
                text += L"h\u2005";
            }
            if (hms.minutes().count()) [[likely]] {
                text += fast_io::wconcat(hms.minutes().count());
                text += L"m\u2005";
            }
            if (hms.seconds().count()) [[likely]] {
                text += fast_io::wconcat(hms.seconds().count());
                text += L's';
            }

            DurationCount().Text(text);
        }
        // set level change button
        {
            auto level{ FolderLevel() };
            if (path_stack_.empty()) [[unlikely]] {
                level.Content(winrt::box_value(L"\\"));
                }
            else {
                level.Content(winrt::box_value(path_stack_.back()));
            }
            auto flyout{ level.Flyout().try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyout>() };
            auto items{ flyout.Items() };
            items.Clear();
            auto event{ [&self = *this](winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&) {
                    auto current{ sender.as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Text() };
                    if (current[0] == L'\\') [[unlikely]] {
                        if (self.path_stack_.empty()) return;
                        self.path_stack_.clear();
                        self.folders_stack_.clear();
                        self.BuildRoot();
                        }
                    else {
                        auto count{ 0uz };
                        for (auto const& name : std::views::reverse(self.path_stack_)) [[likely]] {
                            if (name != current) [[likely]]
                                ++count;
                            else
                                break;
                            }
                        self.path_stack_.resize(count);
                        self.folders_stack_.resize(count);
                        self.Rebuild();
                    }
                    for (auto const& name : self.folders_view_.GetView()) {
                        if (name == current) {
                            self.FolderViewList().ScrollIntoView(winrt::box_value(name));
                            break;
                        }
                    }
                    }
            };
            auto root{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
            root.Text(L"\\");
            root.Tapped(event);
            items.Append(root);
            for (auto const& name : path_stack_) [[likely]] {
                auto item{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
                item.Text(name);
                items.Append(item);
                item.Tapped(event);
            }
        }
    }
    void FolderView::BuildRoot() {
        // for music_view_
        auto musicContainer{ std::vector<winrt::Player::InfoViewModel>{} };
        auto set{ std::set<std::wstring_view>{} };
        {
            musicContainer.reserve(20uz);

            // init all data
            for (auto const& info : library_) [[likely]] {
                auto const path{ std::wstring_view{info.Path} };
                auto const end{ path.find(L'\\', 2uz) };

                // if is file
                if (end == decltype(path)::npos)
                    musicContainer.emplace_back(info);
                // if is folder
                else
                    set.emplace(path.begin() + 1uz, path.begin() + end);
            }
        }
            // for folders_view_
        auto foldersContainer{ std::vector<winrt::hstring>{} };
        {
            foldersContainer.reserve(20uz);
            for (auto const& i : set) [[likely]]
                foldersContainer.emplace_back(i);
        }

        UpdateUI(foldersContainer, musicContainer);
    }
    void FolderView::Build() {

        auto fullpath{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };
        // for music_view_
        auto musicContainer{ std::vector<winrt::Player::InfoViewModel>{} };
        // for current stack frame
        auto frame{ std::vector<std::reference_wrapper<const winrt::Data::Music>>{} };
        {
            musicContainer.reserve(20uz);
            frame.reserve(50uz);

            if (folders_stack_.empty()) [[unlikely]] {
                // current is root
                for (auto const& info : library_) [[likely]] {
                    auto const path{ std::wstring_view{info.Path} };
                    if (path.find(fullpath, 1uz) != decltype(path)::npos)
                        frame.emplace_back(info);
                    }
                }
            else {
                for (auto const& info : folders_stack_.back()) [[likely]] {
                    auto const path{ std::wstring_view{info.get().Path}};

                    if (path.find(fullpath, 1uz) != decltype(path)::npos)
                        frame.emplace_back(info);
                    }
            }
        }
        auto set{ std::set<std::wstring_view>{} };
        for (auto const& i : frame) [[likely]] {
            auto const& info{ i.get() };
            auto const path{ std::wstring_view{info.Path}.substr(fullpath.size() + 1uz) };
            auto const end{ path.find(L'\\', 2uz) };

            // if is file
            if (end == decltype(path)::npos)
                musicContainer.emplace_back(info);
            // if is folder
            else
                set.emplace(path.begin() + 1uz, path.begin() + end);
        }
        // for folders_view_
        auto foldersContainer{ std::vector<winrt::hstring>{} };
        {
            foldersContainer.reserve(20uz);
            foldersContainer.emplace_back(L".\u2005.\u2005.\u2005");
            for (auto const& info : set) [[likely]]
                foldersContainer.emplace_back(info);
                
            folders_stack_.push_back(std::move(frame));
        }

        UpdateUI(foldersContainer, musicContainer);
    }
    void FolderView::Rebuild() {

        // for music_view_
        auto musicContainer{ std::vector<winrt::Player::InfoViewModel>{} };
        auto set{ std::set<std::wstring_view>{} };
        {
            musicContainer.reserve(20uz);

            auto const& frame{ folders_stack_.back() };
            auto fullpath{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };

            for (auto const& i : frame) [[likely]] {
                auto const& info{ i.get() };
                auto const path{ std::wstring_view{info.Path}.substr(fullpath.size() + 1uz) };
                auto const end{ path.find(L'\\', 2uz) };

                // if is file
                if (end == decltype(path)::npos)
                    musicContainer.emplace_back(info);
                // if is folder
                else
                    set.emplace(path.begin() + 1uz, path.begin() + end);
            }
        }

        // for folders_view_
        auto foldersContainer{ std::vector<winrt::hstring>{} };
        {
            foldersContainer.reserve(20uz);
            foldersContainer.emplace_back(L".\u2005.\u2005.\u2005");
            for (auto const& info : set) [[likely]]
                foldersContainer.emplace_back(info);
        }

        UpdateUI(foldersContainer, musicContainer);
    }
    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FolderView::FolderList() {
        return folders_view_;
    }
    winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> FolderView::MusicList() {
        return music_view_;
    }
}
