# PlayerWinRT

PlayerWinRT is an open source local music player intended for the modern Windows experience.

PlayerWinRT is developed using WinUI 3, C++/WinRT and C++ 23.

The project is currently in early stage.

## Contributing

All kinds of contributions will be appreciated. All suggestions, pull requests and issues are welcome.

If you want to contribute your own code, you can simply submit a plain pull request explaining you changes.
For larger and complex contributions it would be nice if you could open an issue before starting to work on it.

## Features

This list is intended as design goals only.

- WAV, WMA, MP3, FLAC, ALAC, AC3 and other formats and codecs supported, refer to [Supported codecs](https://learn.microsoft.com/zh-cn/windows/uwp/audio-video-camera/supported-codecs)
- Equalizer
- Timer
- Network storage such as SMB, FTP, WebDAV
- OneDrive

## System Requirements

- Supported OS: Windows 10 Version 1809 or later and Windows 11.
- Supported Platforms: x64 and ARM64.

## Build

Only guaranteed to compile on the latest VS2022, but welcome to provide forward support, as well as to provide testing of compatibility with older toolchains.

## Roadmap

- April: basic UI and playback functions
- May  : i18n and basic network storage
- June : full network storage and OneDrive