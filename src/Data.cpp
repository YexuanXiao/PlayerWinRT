#include "pch.h"

#include "Data.h"

namespace Data {
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Storage::StorageFile>> GetFileListFromFolder(winrt::Windows::Storage::StorageFolder const& folder) {
		// extension list, must start with a dot
		auto extensions{ std::vector<winrt::hstring>{L".flac",L".wav",L".alac",L".ac3",L".mp3",L".wma"} };
		// the second parameter of ctor of QueryOptions only needs to be iterable
		auto options{ winrt::Windows::Storage::Search::QueryOptions{winrt::Windows::Storage::Search::CommonFileQuery::OrderByName, extensions} };
		auto query{ folder.CreateFileQueryWithOptions(options) };
		// return as is, no need to make this function a coroutine
		return query.GetFilesAsync();
	}
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::JsonObject> GetLibraryFromFolderPath(winrt::hstring const& name, winrt::hstring const& protocol, winrt::hstring path, winrt::hstring const& icon) {
		auto const& list{ co_await GetFileListFromFolder(co_await winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(path)) };
		auto array{ winrt::Windows::Data::Json::JsonArray{} };
		for (auto file : list) [[likely]] {
			// reopen because the file from filter forbit to access props
			file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(file.Path());
			auto const prop{ co_await file.Properties().GetMusicPropertiesAsync() };
			auto const genres{ prop.Genre() };
			auto const genre{ winrt::hstring{std::views::join_with(genres, L' ') | std::ranges::to<std::basic_string>()} };
			auto item{ winrt::Windows::Data::Json::JsonObject{} };
			item.SetNamedValue(L"Album", winrt::Windows::Data::Json::JsonValue::CreateStringValue(prop.Album()));
			item.SetNamedValue(L"Albumartist", winrt::Windows::Data::Json::JsonValue::CreateStringValue(prop.AlbumArtist()));
			item.SetNamedValue(L"Artist", winrt::Windows::Data::Json::JsonValue::CreateStringValue(prop.Artist()));
			item.SetNamedValue(L"Bitrate", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(prop.Bitrate()));
			item.SetNamedValue(L"Duration", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(static_cast<double>(prop.Duration().count())));
			item.SetNamedValue(L"Genre", winrt::Windows::Data::Json::JsonValue::CreateStringValue(genre));
			// considering that users may migrate library, delete redundant root paths
			auto filepath{ file.Path() };
			item.SetNamedValue(L"Path", winrt::Windows::Data::Json::JsonValue::CreateStringValue(std::wstring_view{filepath.begin() + path.size(), filepath.end()}));
			item.SetNamedValue(L"Title", winrt::Windows::Data::Json::JsonValue::CreateStringValue(prop.Title()));
			item.SetNamedValue(L"Track", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(prop.TrackNumber()));
			item.SetNamedValue(L"Year", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(prop.Year()));
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
	std::vector<winrt::Data::MusicInfo> TramsformJsonArrayToVector(winrt::Windows::Data::Json::JsonArray const& array) {
		auto size{ std::size_t{array.Size()} };
		auto vector{ std::vector<winrt::Windows::Data::Json::JsonValue>{size, nullptr} };
		array.GetMany(0, vector);
		auto result{ std::vector<winrt::Data::MusicInfo>{} };
		result.reserve(size);
		for (auto const& i: vector) {
			auto info{ i.GetObjectW() };
			auto item{ winrt::Data::MusicInfo{} };
			item.Album = info.GetNamedString(L"Album");
			item.Albumartist = info.GetNamedString(L"Albumartist");
			item.Artist = info.GetNamedString(L"Albumartist");
			item.Bitrate = static_cast<decltype(item.Bitrate)>(info.GetNamedNumber(L"Bitrate"));
			item.Duration = static_cast<decltype(item.Duration)>(info.GetNamedNumber(L"Duration"));
			item.Genre = info.GetNamedString(L"Genre");
			item.Path = info.GetNamedString(L"Path");
			item.Title = info.GetNamedString(L"Title");
			item.Track = static_cast<decltype(item.Track)>(info.GetNamedNumber(L"Track"));
			item.Year = static_cast<decltype(item.Year)>(info.GetNamedNumber(L"Year"));
			result.emplace_back(std::move(item));
		}
		return result;
	}
}