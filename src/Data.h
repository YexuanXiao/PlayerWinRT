#pragma once
#include "pch.h"

#include <winrt/Data.h>

namespace Data {
    std::vector<winrt::Data::Music> TramsformJsonArrayToVector(winrt::Windows::Data::Json::JsonArray const& array);
    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::JsonObject> GetLibraryFromFolderPath(winrt::hstring const& name, winrt::hstring const& protocol,winrt::hstring path, winrt::hstring const& icon);
	struct PlayList {
		using String = std::wstring;
		String name;
		std::vector<winrt::Data::Music> list;
	};
	using Equalizer = std::array<int8_t, 10>;

	inline auto sample{ R"(
{
    "library":[
        {
            "name":"system",
            "list":[{
                "title":"",
                "album":"",
                "genre":"",
                "artist":"",
                "albumartist":"",
                "year":"",
                "path":"",
                "duration":0,
                "bitrate":0,
                "track":0,
                "samplerate":0,
                "cd":0
            }
            ]
        },
        {
            "name":"xxxx",
            "list":[

            ]
        }
    ],
    "current":{
        "library":"",
        "path":"",
        "index":0,
        "list":[

        ]
    }
}
)" };
}

namespace Data::Global {
    inline winrt::Data::Library CurrentLibrary{};
}