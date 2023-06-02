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

        auto container{ std::vector<winrt::Player::InfoViewModel>{} };
        container.reserve(info_list_.Size());
        for (auto info : info_list_)
        {
            container.emplace_back(info);
        }
        music_view_.ReplaceAll(container);
    }

    void NowPlaying::OnNavigatingFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigatingCancelEventArgs const&)
    {
        player_view_model_root_.PropertyChanged(sync_pvm_);
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::Player::InfoViewModel> NowPlaying::MusicList()
    {
        return music_view_;
    }

    void NowPlaying::ListView_ItemClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& e)
    {
    }

    void NowPlaying::Remove_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
    }
}
