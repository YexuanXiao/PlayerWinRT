#include "pch.h"

#include "data.h"

#include <winrt/Windows.Data.Json.h>

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
}