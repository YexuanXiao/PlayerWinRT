#pragma once
#include "pch.h"

namespace Player::Data {
	struct MusicInfo {
		using String = std::wstring;
		String title;
		String album;
		String genre;
		String artist;
		String albumartist;
		String year;
		String path;
		unsigned long long coverhash;
		unsigned long long duration;
		int bitrate;
		int track;
		int samplerate;
		int cd;
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
}
