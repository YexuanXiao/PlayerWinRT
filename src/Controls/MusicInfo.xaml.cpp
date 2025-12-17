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
                using namespace std::chrono_literals;
                co_await 1s;
                auto graph{ winrt::Windows::Media::Audio::AudioGraph::CreateAsync(winrt::Windows::Media::Audio::AudioGraphSettings{ winrt::Windows::Media::Render::AudioRenderCategory::Media }).get() };
                auto file{ winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(self.Path()).get() };
                auto result{ graph.Graph().CreateFileInputNodeAsync(file).get() };
                auto properties{ result.FileInputNode().EncodingProperties() };
                co_await ui_thread;
                self.SampleRate().Text(util::hformat(L"{}Hz", properties.SampleRate()));
                self.BitDepth().Text(util::hformat(L"{}", properties.BitsPerSample()));
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
        return util::hformat(L"{}", music_.Track);
    }

    winrt::hstring MusicInfo::Duration()
    {
        auto const hms{ std::chrono::hh_mm_ss{ winrt::clock::duration{ static_cast<int64_t>(music_.Duration) } } };
        auto hours{ hms.hours().count() };
        if (hours)
            return util::hformat(L"{:02}:{:02}:{:02}", hours, hms.minutes().count(), hms.seconds().count());
        else
            return util::hformat(L"{:02}:{:02}", hms.minutes().count(), hms.seconds().count());
    }

    winrt::hstring MusicInfo::Genre()
    {
        return music_.Genre;
    }

    winrt::hstring MusicInfo::Year()
    {
        return util::hformat(L"{}", music_.Year);
    }

    winrt::hstring MusicInfo::Bitrate()
    {
        return util::hformat(L"{}Kbps", music_.Bitrate / 1000);
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
        Win32Helper::OpenExplorer(std::format(L"{}{}", library_.address, path.substr(0, path.find_last_of(L'\\'))));
    }
}
