#include "pch.h"

#include "Data.h"

namespace Data {
	bool CheckLibraryLegal(Data::Library const& value) {
		if (value.address.empty() && value.name.empty()) return false;
		return true;
	}
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Storage::StorageFile>> GetFileListFromFolder(winrt::Windows::Storage::StorageFolder const& folder) {
		// extension list, must start with a dot
		auto extensions{ std::vector<winrt::hstring>{L".flac",L".wav",L".alac",L".ac3",L".mp3",L".wma"} };
		// the second parameter of ctor of QueryOptions only needs to be iterable
		auto options{ winrt::Windows::Storage::Search::QueryOptions{winrt::Windows::Storage::Search::CommonFileQuery::OrderByName, extensions} };
		auto query{folder.CreateFileQueryWithOptions(options)};
		// return as is, no need to make this function a coroutine
		return query.GetFilesAsync();
	}
	concurrency::task<std::vector<Data::MusicInfo>> GetListWithMusicInfoFromFolder(winrt::Windows::Storage::StorageFolder const& folder) {
		auto const& files{ co_await GetFileListFromFolder(folder) };
		auto list{ std::vector<Data::MusicInfo>{} };
		list.reserve(files.Size());
		for (auto file : files) [[likely]] {
			// reopen because the file from filter forbit to access props
			file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(file.Path());
			auto const& prop{ co_await file.Properties().GetMusicPropertiesAsync() };
			auto const& genres{ prop.Genre() };
			auto const genre{ winrt::hstring{std::views::join_with(genres, L' ') | std::ranges::to<std::basic_string>()} };
			auto info{ Data::MusicInfo{prop.Title(), prop.Album(), genre, prop.Artist(), prop.AlbumArtist(), file.Path(), prop.Duration().count(), prop.Year(), prop.Bitrate(), prop.TrackNumber()} };
			list.emplace_back(info);
		}
		co_return list;
	}
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::JsonObject> GetLibraryFromFolderPath(winrt::hstring const& name, winrt::hstring const& protocol, winrt::hstring const& path, winrt::hstring const& icon) {
		auto const& list{ co_await GetListWithMusicInfoFromFolder(co_await winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(path)) };
		auto array{ winrt::Windows::Data::Json::JsonArray{} };
		for (auto const& info : list) [[likely]] {
			auto item{ winrt::Windows::Data::Json::JsonObject{} };
			item.SetNamedValue(L"Album", winrt::Windows::Data::Json::JsonValue::CreateStringValue(info.Album));
			item.SetNamedValue(L"Albumartist", winrt::Windows::Data::Json::JsonValue::CreateStringValue(info.Albumartist));
			item.SetNamedValue(L"Artist", winrt::Windows::Data::Json::JsonValue::CreateStringValue(info.Artist));
			item.SetNamedValue(L"Bitrate", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(info.Bitrate));
			item.SetNamedValue(L"Duration", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(static_cast<double>(info.Duration)));
			item.SetNamedValue(L"Genre", winrt::Windows::Data::Json::JsonValue::CreateStringValue(info.Genre));
			// considering that users may migrate library, delete redundant root paths
			auto filepath{ std::wstring_view{info.Path.begin() + path.size(),info.Path.size() - path.size()} };
			item.SetNamedValue(L"Path", winrt::Windows::Data::Json::JsonValue::CreateStringValue(filepath));
			item.SetNamedValue(L"Title", winrt::Windows::Data::Json::JsonValue::CreateStringValue(info.Title));
			item.SetNamedValue(L"Track", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(info.Track));
			item.SetNamedValue(L"Year", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(info.Year));
			array.Append(item);
		}
		auto result{ winrt::Windows::Data::Json::JsonObject{} };
		result.SetNamedValue(L"List", array);
		result.SetNamedValue(L"Path", winrt::Windows::Data::Json::JsonValue::CreateStringValue(path));
		result.SetNamedValue(L"Name", winrt::Windows::Data::Json::JsonValue::CreateStringValue(name));
		result.SetNamedValue(L"Protocol", winrt::Windows::Data::Json::JsonValue::CreateStringValue(protocol));
		result.SetNamedValue(L"Icon", winrt::Windows::Data::Json::JsonValue::CreateStringValue(icon));
		co_return result;
	}
}