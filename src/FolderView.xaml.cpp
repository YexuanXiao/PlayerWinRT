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
            auto path{ self.folders_view_.GetAt(index) };
            if (path == L"...") [[unlikely]] {
                // back to previous level
                self.folders_stack_.pop_back();
                self.path_stack_.pop_back();
                if (self.path_stack_.empty()) {
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

        // because constructor cannot be coroutine, so initialize in Loaded event
    }
    winrt::Windows::Foundation::IAsyncAction FolderView::FolderView_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        // event trigged when frame backed and constructed
        if (library_.empty()) {
            // after constructed
            library_ = ::Data::TramsformJsonArrayToVector((co_await SettingsHelper::GetLibaray(::Data::Global::CurrentLibrary.name)));
            BuildRoot();
        }
    }
    uint32_t FolderView::CalculateTrueFolderCount(uint32_t value) {
        auto adjust{ path_stack_.empty() ? 0 : 1 };
        return value - adjust;
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
            auto level{ FolderLevel() };
            if (path_stack_.empty()) {
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
                    if (current[0] == L'\\') {
                        if (self.path_stack_.empty()) return;
                        self.path_stack_.clear();
                        self.folders_stack_.clear();
                        self.BuildRoot();
                        return;
                    }
                    for (auto const& name : std::views::reverse(self.path_stack_)) {
                        if (name != current) {
                            self.path_stack_.pop_back();
                            self.folders_stack_.pop_back();
                        }
                        else {
                            self.Rebuild();
                            return;
                        }
                    }
                    } };
            auto root{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
            root.Text(L"\\");
            root.Tapped(event);
            items.Append(root);
            for (auto const& name : path_stack_) {
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
        for (auto const& i : library_) {
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
        for (auto const& i : set) {
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
            for (auto const& info : library_) {
                auto const path{ std::wstring_view{info.Path} };
                auto const end{ path.find(fullpath, 1uz) };
                if (end != decltype(path)::npos) {
                    frame.emplace_back(info);
                }
            }
            }
        else {
            for (auto const& i : folders_stack_.back()) {
                auto const& info{ i.get() };
                auto const path{ std::wstring_view{info.Path} };
                auto const end{ path.find(fullpath, 1uz) };
                if (end != decltype(path)::npos) {
                    frame.emplace_back(i);
                }
            }
        }

        auto set{ std::set<std::wstring_view>{} };
        for (auto const& i : frame) {
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
        foldersContainer.emplace_back(L"...");
        for (auto const& i : set) {
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
        for (auto const& i : frame) {
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
        foldersContainer.emplace_back(L"...");
        for (auto const& i : set) {
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