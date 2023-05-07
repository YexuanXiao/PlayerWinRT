#pragma once
#include "MusicInfoModel.g.h"

namespace winrt::Player::implementation
{
    struct MusicInfoModel : MusicInfoModelT<MusicInfoModel>
    {
        MusicInfoModel() = default;
        MusicInfoModel(Player::Data::MusicInfo const&);

        Player::Data::MusicInfo Info();
        void Info(Player::Data::MusicInfo const&);
    private:
        Player::Data::MusicInfo info_;
    };
}
namespace winrt::Player::factory_implementation
{
    struct MusicInfoModel : MusicInfoModelT<MusicInfoModel, implementation::MusicInfoModel>
    {
    };
}
