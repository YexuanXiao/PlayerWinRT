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
        // because constructor cannot be coroutine, so initialize in Loaded event
    }
    winrt::Windows::Foundation::IAsyncAction FolderView::FolderView_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        library_ = ::Data::TramsformJsonArrayToVector((co_await SettingsHelper::GetLibaray(::Data::Global::CurrentLibrary.name)));

        BuildRoot();

        // init FolderList UI items event
        FolderViewList().SelectionChanged([&self = *this](IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&) {
            // don't use ItemClick event because the return type of args.SelectedItem() is unknown
            auto index{ self.FolderViewList().SelectedIndex() };
            if (index == -1) return; // workaround beacuse one click trigged event twice
            auto path{ self.folders_view_.GetAt(index)};
            if (path == L"...") [[unlikely]] {
                // back to previous level
                self.folders_stack_.pop_back();
                self.path_stack_.pop_back();
                if (self.path_stack_.empty()) {
                    // backed is root
                    self.BuildRoot();
                }
                else {
                    // bcaked not root
                    self.Rebuild();
                }
                }
            else {
                // fowward to next level
                self.path_stack_.push_back(path);
                self.Build();
            }
            });
    }
    void FolderView::UpdateCountedUI(std::size_t files, std::size_t music) {
        FolderCount().Text(fast_io::wconcat_winrt_hstring(files));
        MusicCount().Text(fast_io::wconcat_winrt_hstring(music));
    }
    void FolderView::BuildRoot() {
        // for files_view_
        auto filesContainer{ std::vector<winrt::hstring>{} };
        filesContainer.reserve(20uz);
        // for music_view_
        auto musicContainer{ std::vector<winrt::Player::InfoViewModel>{} };
        musicContainer.reserve(20uz);

        // init all data
        {
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
            for (auto const& i : set) {
                filesContainer.emplace_back(i);
            }
        }

        folders_view_.ReplaceAll(filesContainer);
        music_view_.ReplaceAll(musicContainer);
        UpdateCountedUI(filesContainer.size(), musicContainer.size());
    }
    void FolderView::Build() {
        // for files_view_
        auto filesContainer{ std::vector<winrt::hstring>{} };
        filesContainer.reserve(20uz);
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
                    frame.emplace_back(info);
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
        filesContainer.emplace_back(L"...");
        for (auto const& i : set) {
            filesContainer.emplace_back(i);
        }
        folders_stack_.push_back(std::move(frame));

        folders_view_.ReplaceAll(filesContainer);
        music_view_.ReplaceAll(musicContainer);
        UpdateCountedUI(filesContainer.size() - 1uz, musicContainer.size());
    }
    void FolderView::Rebuild() {
        // for files_view_
        auto filesContainer{ std::vector<winrt::hstring>{} };
        filesContainer.reserve(20uz);
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
        filesContainer.emplace_back(L"...");
        for (auto const& i : set) {
            filesContainer.emplace_back(i);
        }

        folders_view_.ReplaceAll(filesContainer);
        music_view_.ReplaceAll(musicContainer);
        UpdateCountedUI(filesContainer.size() - 1uz, musicContainer.size());
    }
    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> FolderView::FolderList() {
        return folders_view_;
    }
    winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> FolderView::MusicList() {
        return music_view_;
    }
}