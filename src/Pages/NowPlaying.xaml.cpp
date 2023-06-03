#include "pch.h"
#include "NowPlaying.xaml.h"
#if __has_include("NowPlaying.g.cpp")
#include "NowPlaying.g.cpp"
#endif

#include <winrt/Data.h>

namespace winrt::Player::implementation
{
    NowPlaying::NowPlaying()
    {
        InitializeComponent();
    }

    winrt::Player::PlayerViewModel NowPlaying::PlayerViewModel()
    {
        return player_view_model_;
    }

    winrt::Windows::Foundation::IAsyncAction NowPlaying::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
    {
        if (args.Parameter() == nullptr)
            co_return;

        auto argument{ args.Parameter().try_as<winrt::Data::ControlPageParameter>() };
        player_view_model_root_ = argument.PlayerViewModel();
        info_list_ = argument.Music();
        play_list_ = argument.List();
        music_list_ = play_list_.Items();

        player_view_model_.Image(player_view_model_root_.Image());
        player_view_model_.Album(player_view_model_root_.Album());
        player_view_model_.Title(player_view_model_root_.Title());
        player_view_model_.Artist(player_view_model_root_.Artist());

        auto container{ std::vector<winrt::Player::InfoViewModel>{} };
        container.reserve(info_list_.Size());
        for (auto info : info_list_)
        {
            container.emplace_back(info);
        }
        MusicCount().Text(fast_io::wconcat_winrt_hstring(container.size()));
        music_view_ = winrt::single_threaded_observable_vector(std::move(container));
        // sync player_view_model_root_ to player_view_model_
        sync_pvm_ = player_view_model_root_.PropertyChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
            if (args.PropertyName() == L"Position")
                return;
            if (args.PropertyName() == L"Image")
                self.player_view_model_.Image(self.player_view_model_root_.Image());
            else if (args.PropertyName() == L"Album")
                self.player_view_model_.Album(self.player_view_model_root_.Album());
            else if (args.PropertyName() == L"Title")
                self.player_view_model_.Title(self.player_view_model_root_.Title());
            else if (args.PropertyName() == L"Artist")
                self.player_view_model_.Artist(self.player_view_model_root_.Artist());
        });
        // sync play_list_ to music_view_
        music_view_.GetAt(play_list_.CurrentItemIndex()).SetState(true);
        sync_pl_ = play_list_.CurrentItemChanged([&self = *this, ui_thread = winrt::apartment_context{}](decltype(play_list_) const& sender, winrt::Windows::Media::Playback::CurrentMediaPlaybackItemChangedEventArgs const&) -> winrt::Windows::Foundation::IAsyncAction {
            auto item{ sender.CurrentItem() };
            if (item == nullptr)
                co_return;
            co_await ui_thread;
            auto info{ self.info_list_.GetAt(sender.CurrentItemIndex()) };
            for (auto element : self.music_view_)
            {
                if (info.Duration == element.Duration())
                {
                    element.SetState(true);
                }
                else
                {
                    element.SetState(false);
                }
            }
        });
        // regist events
        sync_mv_ = music_view_.VectorChanged([&self = *this, strong = this->get_strong(), ui_thread = winrt::apartment_context{}](decltype(music_view_) const&, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& args) -> winrt::Windows::Foundation::IAsyncAction {
            auto operate{ args.CollectionChange() };
            auto index{ args.Index() };
            switch (operate)
            {
            case decltype(operate)::ItemRemoved: {
                if (self.index_ == std::numeric_limits<decltype(index_)>::max())
                {
                    // actual remove
                    self.count_ -= self.info_list_.GetAt(index).Duration;
                    self.info_list_.RemoveAt(index);
                    self.play_list_.Items().RemoveAt(index);
                    self.MusicCount().Text(fast_io::wconcat_winrt_hstring(self.info_list_.Size()));
                    self.index_ = 0u;
                }
                else
                {
                    // fake remove
                    self.index_ = index;
                    self.removed_music_ = self.info_list_.GetAt(index);
                    self.removed_item_ = self.music_list_.GetAt(index);
                }
                break;
            }
            case decltype(operate)::ItemChanged: {
                break;
            }
            case decltype(operate)::ItemInserted: {
                self.info_list_.RemoveAt(self.index_);
                self.info_list_.InsertAt(index, self.removed_music_);
                self.play_list_.Items().RemoveAt(self.index_);
                self.play_list_.Items().InsertAt(index, self.removed_item_);
                break;
            }
            case decltype(operate)::Reset:
                auto count{ winrt::clock::duration{} };
                for (auto const& info : self.info_list_) [[likely]]
                    count += winrt::clock::duration{ info.Duration };
                self.count_ = count.count();
                break;
            }
            auto text{ winrt::hstring{} };
            auto const hms{ std::chrono::hh_mm_ss{ winrt::clock::duration{ self.count_ } } };
            if (hms.hours().count()) [[likely]]
                text = fast_io::wconcat_winrt_hstring(hms.hours(), fast_io::manipulators::chvw(L'\u2005'), hms.minutes(), fast_io::manipulators::chvw(L'\u2005'), hms.seconds());
            else if (hms.minutes().count())
                text = fast_io::wconcat_winrt_hstring(hms.minutes(), fast_io::manipulators::chvw(L'\u2005'), hms.seconds());
            co_await ui_thread;
            self.DurationCount().Text(text);
        });
        MusicViewList().ItemsSource(music_view_);
    }

    void NowPlaying::OnNavigatingFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigatingCancelEventArgs const&)
    {
        player_view_model_root_.PropertyChanged(sync_pvm_);
        play_list_.CurrentItemChanged(sync_pl_);
        music_view_.VectorChanged(sync_mv_);
    }

    void NowPlaying::ListView_ItemClick(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& args)
    {
        auto index{ decltype(info_list_.Size()){} };
        auto duration{ args.ClickedItem().try_as<winrt::Player::InfoViewModel>().Get().Duration };
        for (auto const& info : info_list_.GetView())
        {
            if (info.Duration != duration)
                ++index;
            else
                break;
        }
        play_list_.MoveTo(index);
    }

    void NowPlaying::Remove_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto index{ decltype(info_list_.Size()){} };
        auto duration{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>().Tag().try_as<winrt::Player::InfoViewModel>().Get().Duration };
        for (auto const& info : info_list_.GetView())
        {
            if (info.Duration != duration)
                ++index;
            else
                break;
        }
        index_ = std::numeric_limits<decltype(index_)>::max();
        music_view_.RemoveAt(index);
    }
}
