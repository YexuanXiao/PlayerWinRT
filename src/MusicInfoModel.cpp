#include "pch.h"
#include "MusicInfoModel.h"
#include "MusicInfoModel.g.cpp"

namespace winrt::Player::implementation
{
    winrt::Player::Data::MusicInfo MusicInfoModel::Info()
    {
        return info_;
    }
    void MusicInfoModel::Info(winrt::Player::Data::MusicInfo const& value)
    {
        info_ = value;
    }
    MusicInfoModel::MusicInfoModel(winrt::Player::Data::MusicInfo const& value)
    {
        info_ = value;
    }
}
