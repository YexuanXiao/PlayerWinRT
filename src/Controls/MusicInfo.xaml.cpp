#include "pch.h"
#include "MusicInfo.xaml.h"
#if __has_include("MusicInfo.g.cpp")
#include "MusicInfo.g.cpp"
#endif

#include <winrt/Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Audio.h>
#include <winrt/Windows.Media.MediaProperties.h>

#include <Win32Helper.h>

namespace winrt::Player::implementation
{
    MusicInfo::MusicInfo(winrt::Data::MusicInfoParameter const& para) : library_(para.Library), music_(para.Music)
    {
        InitializeComponent();

        auto resource{ winrt::Windows::ApplicationModel::Resources::ResourceLoader{} };
        Title(winrt::box_value(resource.GetString(L"MusicInfo/Text")));
        CloseButtonText(resource.GetString(L"Close"));

        if (library_.protocol == L"local")
        {
            Loaded([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
                auto ui_thread{ winrt::apartment_context{} };
                co_await winrt::resume_background();
                auto graph{ winrt::Windows::Media::Audio::AudioGraph::CreateAsync(winrt::Windows::Media::Audio::AudioGraphSettings{ winrt::Windows::Media::Render::AudioRenderCategory::Media }).get() };
                auto file{ winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(self.Path()).get() };
                auto result{ graph.Graph().CreateFileInputNodeAsync(file).get() };
                auto properties{ result.FileInputNode().EncodingProperties() };
                co_await ui_thread;
                self.SampleRate().Text(fast_io::wconcat_winrt_hstring(properties.SampleRate(), L"Hz"));
                self.BitDepth().Text(fast_io::wconcat_winrt_hstring(properties.BitsPerSample()));
            });
        }
    }

    winrt::hstring MusicInfo::MusicTitle()
    {
        return music_.Title;
    }

    winrt::hstring MusicInfo::Artist()
    {
        return music_.Artist;
    }

    winrt::hstring MusicInfo::Album()
    {
        return music_.Album;
    }

    winrt::hstring MusicInfo::Albumartist()
    {
        return music_.Albumartist;
    }

    winrt::hstring MusicInfo::Track()
    {
        return fast_io::wconcat_winrt_hstring(music_.Track);
    }

    winrt::hstring MusicInfo::Duration()
    {
        auto const hms{ std::chrono::hh_mm_ss{ winrt::clock::duration{ static_cast<int64_t>(music_.Duration) } } };
        auto hours{ hms.hours().count() };
        if (hours)
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hours, 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
        else
            return fast_io::wconcat_winrt_hstring(fast_io::mnp::right(hms.minutes().count(), 2, L'0'), fast_io::mnp::chvw(L':'), fast_io::mnp::right(hms.seconds().count(), 2, L'0'));
    }

    winrt::hstring MusicInfo::Genre()
    {
        return music_.Genre;
    }

    winrt::hstring MusicInfo::Year()
    {
        return fast_io::wconcat_winrt_hstring(music_.Year);
    }

    winrt::hstring MusicInfo::Bitrate()
    {
        return fast_io::wconcat_winrt_hstring(music_.Bitrate / 1000, L"kbps");
    }

    winrt::hstring MusicInfo::Format()
    {
        auto path{ std::wstring_view{ music_.Path } };

        return winrt::hstring{ path.substr(path.find_last_of(L'.'), path.size()) };
    }

    winrt::hstring MusicInfo::Path()
    {
        return library_.address + music_.Path;
    }

    void MusicInfo::Folder_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto path{ std::wstring_view{ music_.Path } };
        Win32Helper::OpenExplorer(fast_io::wconcat(library_.address, path.substr(0, path.find_last_of(L'\\'))));
    }
}
