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
        player_view_model_root_.PropertyChanged([&self = *this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
            // volume change
            if (args.PropertyName() == L"Image")
                self.player_view_model_.Image(self.player_view_model_root_.Image());
            if (args.PropertyName() == L"Album")
                self.player_view_model_.Album(self.player_view_model_root_.Album());
            if (args.PropertyName() == L"Title")
                self.player_view_model_.Title(self.player_view_model_root_.Title());
            if (args.PropertyName() == L"Artist")
                self.player_view_model_.Artist(self.player_view_model_root_.Artist());
        });
    }

    winrt::Player::PlayerViewModel NowPlaying::PlayerViewModel()
    {
        return player_view_model_;
    }
}
