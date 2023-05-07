#pragma once
#include "pch.h"

namespace Player::Data {
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
	struct PlayList {
		using String = std::wstring;
		String name;
		std::vector<MusicInfo> list;
	};
	struct Library {
		enum class Protocol {
			Local, Samba, WebDAV, FTP
		};
		using String = std::wstring;
		String name;
		Protocol procotol;
		String path;
		String Icon;
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