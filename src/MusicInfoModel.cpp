#include "pch.h"
#include "MusicInfoModel.h"
#include "MusicInfoModel.g.cpp"

namespace winrt::Player::implementation
{
    MusicInfoModel::MusicInfoModel(hstring const& Title_, hstring const& Album_, hstring const& Genre_, hstring const& Artist_, hstring const& Albumartist_, hstring const& Path_, int64_t Duration_, int32_t Bitrate_, int32_t Year_, int32_t Track_):title_(Title_),album_(Album_),genre_(Genre_),artist_(Artist_),albumartist_(Albumartist_),path_(Path_),duration_(Duration_),bitrate_(Bitrate_),year_(Year_),track_(Track_) {

    }
    hstring MusicInfoModel::title()
    {
        return title_;
    }
    void MusicInfoModel::title(hstring const& value)
    {
        title_ = value;
    }
    hstring MusicInfoModel::album()
    {
        return album_;
    }
    void MusicInfoModel::album(hstring const& value)
    {
        album_ = value;
    }
    hstring MusicInfoModel::genre()
    {
        return genre_;
    }
    void MusicInfoModel::genre(hstring const& value)
    {
        genre_ = value;
    }
    hstring MusicInfoModel::artist()
    {
        return artist_;
    }
    void MusicInfoModel::artist(hstring const& value)
    {
        artist_ = value;
    }
    hstring MusicInfoModel::albumartist()
    {
        return albumartist_;
    }
    void MusicInfoModel::albumartist(hstring const& value)
    {
        albumartist_ = value;
    }
    hstring MusicInfoModel::path()
    {
        return path_;
    }
    void MusicInfoModel::path(hstring const& value)
    {
        path_ = value;
    }
    int64_t MusicInfoModel::duration()
    {
        return duration_;
    }
    void MusicInfoModel::duration(int64_t value)
    {
        duration_ = value;
    }
    int32_t MusicInfoModel::year()
    {
        return year_;
    }
    void MusicInfoModel::year(int32_t value)
    {
        year_ = value;
    }
    uint32_t MusicInfoModel::bitrate()
    {
        return bitrate_;
    }
    void MusicInfoModel::bitrate(uint32_t value)
    {
        bitrate_ = value;
    }
    uint32_t MusicInfoModel::track()
    {
        return track_;
    }
    void MusicInfoModel::track(uint32_t value)
    {
        track_ = value;
    }
}
