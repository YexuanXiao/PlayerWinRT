#include "pch.h"
#include "FolderView.xaml.h"
#if __has_include("FolderView.g.cpp")
#include "FolderView.g.cpp"
#endif

#include "SettingsHelper.h"
#include "Data.h"
#include <set>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Player::implementation
{
    FolderView::FolderView()
    {
        InitializeComponent();

        // init FolderList UI items event
        FolderViewList().SelectionChanged([&self = *this](IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
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
        and single selection mode at the same time, and setting options programmatically is invalid,
        but this does not mean that there is no selection effect
        (different from setting the selection mode to None, which does not produce any selection effect),
        so in clicked event handler, cancel the click mode, turn on the single selection mode,
        and then call SelectedItem(item) to trigger the single selection change event.
        This way can achieve both click events and single selection events,
        making it possible to change the ListView UI programmatically without trigger single selection change event
        (because ListView is almost always in click mode,
        so changing selected item programmatically does not trigger single selection events)
        */
        MusicViewList().ItemClick([&self = *this](IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args) {
            self.MusicViewList().SelectedItem(args.ClickedItem());
            self.MusicViewList().IsItemClickEnabled(false);
            });
        MusicViewList().SelectionChanged([&self = *this](IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args) -> winrt::Windows::Foundation::IAsyncAction {
            self.MusicViewList().IsItemClickEnabled(true);
            auto index{ self.MusicViewList().SelectedIndex() };
            if (index == -1) co_return;
            auto size{ self.music_view_.Size()};
            co_await winrt::resume_background();
            auto itemContainer{ std::vector<winrt::Windows::Media::Playback::MediaPlaybackItem>{} };
            itemContainer.reserve(size);
            auto infoContainer{ std::vector<winrt::Data::MusicInfo>{} };
            infoContainer.reserve(size);
            for (auto const& i : self.music_view_.GetView()) {
                auto info{ i.Get() };
                itemContainer.emplace_back(
                   winrt::Windows::Media::Core::MediaSource::CreateFromStorageFile(
                        co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(self.library_info_.address + info.Path)
                   )
                );
                infoContainer.emplace_back(info);
            }
            RootPage::Music().ReplaceAll(itemContainer);
            RootPage::InfoList().ReplaceAll(infoContainer);
            RootPage::List().MoveTo(index);
            RootPage::Library(::Data::Global::CurrentLibrary);
            });
        RootPage::List().CurrentItemChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(RootPage::List()) const& sender, Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            if (sender.CurrentItem() == nullptr) co_return;
            co_await ui_thread;
            auto current{ RootPage::InfoList().GetAt(sender.CurrentItemIndex())};
            for (const auto& info : self.music_view_.GetView()) {
                if (current.Duration == info.Duration()) {
                    self.MusicViewList().SelectedItem(info);
                    break;
                }
            }
            });
        // because constructor cannot be coroutine, so initialize in Loaded event
    }
    winrt::Windows::Foundation::IAsyncAction FolderView::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&) {
        co_await Initialize();
    }
    winrt::Windows::Foundation::IAsyncAction FolderView::Initialize() {
        // event trigged when frame backed and constructed
        if (library_info_ != ::Data::Global::CurrentLibrary)[[unlikely]] {
            library_info_ = ::Data::Global::CurrentLibrary;
            folders_stack_.clear();
            path_stack_.clear();
            folders_view_.Clear();
            music_view_.Clear();
            library_ = ::Data::TramsformJsonArrayToVector((co_await SettingsHelper::GetLibaray(library_info_.name)));
            BuildRoot();
        }
    }
    winrt::Windows::Foundation::IAsyncAction FolderView::FolderView_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        co_await Initialize();
    }
    winrt::hstring FolderView::CalculateTrueFolderCount(uint32_t value) {
        auto adjust{ path_stack_.empty() ? 0 : 1 };
        return fast_io::wconcat_winrt_hstring(value - adjust);
    }
    winrt::hstring FolderView::TransformBitrate(uint32_t value) {
        return fast_io::wconcat_winrt_hstring(value / 1000, L"kbps");
    }
    winrt::hstring FolderView::TransformDuration(int64_t value) {
        auto const hms{ std::chrono::hh_mm_ss{std::chrono::nanoseconds{value * 100}} };
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

        {
            folders_view_.ReplaceAll(folders);
            music_view_.ReplaceAll(music);
        }
        {
            auto fullpath{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };
            fullpath.resize(fullpath.size() + 1uz);
            std::memmove(fullpath.data() + 1uz, fullpath.data(), (fullpath.size() - 1uz) * sizeof(decltype(fullpath)::value_type));
            fullpath[0] = L'\\';
            FullPath().Text(fullpath);
        }
        {
            auto time{ 0ll };
            for (auto const& info : music) [[likely]] {
                time += info.Duration();
            }
            auto const hms{ std::chrono::hh_mm_ss{std::chrono::nanoseconds{time * 100}} };
            auto text{ std::wstring{} };
            text.reserve(20uz);
            using namespace std::literals;
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
        musicContainer.reserve(20uz);

        // init all data
        auto set{ std::set<std::wstring_view>{} };
        for (auto const& i : library_) [[likely]] {
            auto const path{ std::wstring_view{i.Path} };
            auto const end{ path.find(L'\\', 2uz) };

            // if is file
            if (end == decltype(path)::npos) {
                musicContainer.emplace_back(i);
            }
            // if is folder
            else {
                set.emplace(path.begin() + 1uz, path.begin() + end);
            }
        }
        // for folders_view_
        auto foldersContainer{ std::vector<winrt::hstring>{} };
        foldersContainer.reserve(20uz);
        for (auto const& i : set) [[likely]] {
            foldersContainer.emplace_back(i);
        }
        UpdateUI(foldersContainer, musicContainer);
    }
    void FolderView::Build() {
        // for music_view_
        auto musicContainer{ std::vector<winrt::Player::InfoViewModel>{} };
        musicContainer.reserve(20uz);
        // for current stack frame
        auto frame{ std::vector<std::reference_wrapper<const winrt::Data::MusicInfo>>{} };
        frame.reserve(50uz);

        auto fullpath{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };

        if (folders_stack_.empty()) [[unlikely]] {
            // if current is root
            for (auto const& info : library_) [[likely]] {
                auto const path{ std::wstring_view{info.Path} };
                auto const end{ path.find(fullpath, 1uz) };
                if (end != decltype(path)::npos) {
                    frame.emplace_back(info);
                }
            }
            }
        else {
            for (auto const& i : folders_stack_.back()) [[likely]] {
                auto const& info{ i.get() };
                auto const path{ std::wstring_view{info.Path} };
                auto const end{ path.find(fullpath, 1uz) };
                if (end != decltype(path)::npos) {
                    frame.emplace_back(i);
                }
            }
        }

        auto set{ std::set<std::wstring_view>{} };
        for (auto const& i : frame) [[likely]] {
            auto const& info{ i.get() };
            auto const path{ std::wstring_view{info.Path}.substr(fullpath.size() + 1uz) };
            auto const end{ path.find(L'\\', 2uz) };

            // if is file
            if (end == decltype(path)::npos) {
                musicContainer.emplace_back(info);
            }
            // if is folder
            else {
                set.emplace(path.begin() + 1uz, path.begin() + end);
            }
        }
        // for folders_view_
        auto foldersContainer{ std::vector<winrt::hstring>{} };
        foldersContainer.reserve(20uz);
        foldersContainer.emplace_back(L".\u2005.\u2005.\u2005");
        for (auto const& i : set) [[likely]] {
            foldersContainer.emplace_back(i);
        }
        folders_stack_.push_back(std::move(frame));

        UpdateUI(foldersContainer, musicContainer);
    }
    void FolderView::Rebuild() {

        // for music_view_
        auto musicContainer{ std::vector<winrt::Player::InfoViewModel>{} };
        musicContainer.reserve(20uz);

        auto const& frame{ folders_stack_.back() };
        auto fullpath{ path_stack_ | std::views::join_with(L'\\') | std::ranges::to<std::basic_string>() };

        auto set{ std::set<std::wstring_view>{} };
        for (auto const& i : frame) [[likely]] {
            auto const& info{ i.get() };
            auto const path{ std::wstring_view{info.Path}.substr(fullpath.size() + 1uz)};
            auto const end{ path.find(L'\\', 2uz) };

            // if is file
            if (end == decltype(path)::npos) {
                musicContainer.emplace_back(info);
            }
            // if is folder
            else {
                set.emplace(path.begin() + 1uz, path.begin() + end);
            }
        }
        // for folders_view_
        auto foldersContainer{ std::vector<winrt::hstring>{} };
        foldersContainer.reserve(20uz);
        foldersContainer.emplace_back(L".\u2005.\u2005.\u2005");
        for (auto const& i : set) [[likely]] {
            foldersContainer.emplace_back(i);
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