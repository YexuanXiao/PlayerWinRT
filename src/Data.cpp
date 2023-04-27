#include "pch.h"

#include "MusicInfoModel.h"

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.Search.h>

namespace Player::Data {
	winrt::Windows::Data::Json::JsonObject GetFiles;
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Storage::StorageFile>> GetFileListFromFolder(winrt::Windows::Storage::StorageFolder const& folder) {
		auto extensions{ winrt::Windows::Foundation::Collections::IVector<winrt::hstring>{} };
		extensions.Append(L".flac");
		extensions.Append(L".wav");
		extensions.Append(L".alac");
		extensions.Append(L".ac3");
		extensions.Append(L".mp3");
		extensions.Append(L".wma");
		auto options{ winrt::Windows::Storage::Search::QueryOptions{winrt::Windows::Storage::Search::CommonFileQuery::OrderByName, extensions} };
		auto query{folder.CreateFileQueryWithOptions(options)};
		return query.GetFilesAsync();
	}
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::Player::MusicInfoModel>> GetListWithMusicInfoFromFolder(winrt::Windows::Storage::StorageFolder const& folder) {
		auto filelist{ co_await GetFileListFromFolder(folder) };
		std::vector<winrt::Player::MusicInfoModel> infolist{filelist.Size()};
		for (auto i: filelist) {
			auto prop{ co_await i.Properties().GetMusicPropertiesAsync() };
			auto genre{ prop.Genre() };
			auto empty{ !genre.Size() };
			infolist.emplace_back(prop.Title(), prop.Album(), empty ? winrt::hstring{} : *(genre.begin()), prop.Artist(), prop.AlbumArtist(), i.Path(), prop.Duration().count(), prop.Year(), prop.Bitrate(), prop.TrackNumber());
		}
		co_return winrt::single_threaded_vector(std::move(infolist));
	}
}