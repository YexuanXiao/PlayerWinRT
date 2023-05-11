#pragma once
#include "pch.h"

#include <winrt/Data.h>

namespace Data {
	struct MusicInfo {
		using String = winrt::hstring;
		String Title;
		String Album;
		String Genre;
		String Artist;
		String Albumartist;
		String Path;
		int64_t Duration;
        uint32_t Year;
		uint32_t Bitrate;
		uint32_t Track;
	};
    struct Library {
        winrt::hstring name;
        winrt::hstring protocol;
        winrt::hstring address;
        winrt::hstring icon;
    };
    bool CheckLibraryLegal(Data::Library const& value);
    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::JsonObject> GetLibraryFromFolderPath(winrt::hstring const& name, winrt::hstring const& protocol,winrt::hstring const& path, winrt::hstring const& icon);
	struct PlayList {
		using String = std::wstring;
		String name;
		std::vector<MusicInfo> list;
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
    inline winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> Librarys{};
    inline winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::MusicInfo> CurrentView{nullptr};
    inline std::vector<MusicInfo> CurrentLibrary{};
}