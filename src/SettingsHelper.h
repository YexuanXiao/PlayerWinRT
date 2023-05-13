#pragma once
#include "pch.h"

namespace SettingsHelper {

	[[nodiscard]] winrt::Microsoft::UI::Xaml::ElementTheme LoadTheme();
	// for settings page use
	void SetTheme(winrt::Microsoft::UI::Xaml::XamlRoot, winrt::Microsoft::UI::Xaml::ElementTheme);
	// for init
	void SetTheme(winrt::Microsoft::UI::Xaml::FrameworkElement&, winrt::Microsoft::UI::Xaml::ElementTheme);
	[[nodiscard]] bool CheckFirstUse();
	[[nodiscard]] double GetVolume();
	void SetVolume(double);
	enum class Repeat:int32_t {
		None,One,All
	};
	[[nodiscard]] Repeat GetRepeat();
	void SetRepeat(Repeat);
	[[nodiscard]] winrt::Windows::Data::Json::JsonArray GetLibraries();
	[[nodiscard]] winrt::Windows::Foundation::IAsyncAction StoreLibrary(winrt::Windows::Data::Json::JsonObject const& library);
	[[nodiscard]] winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::JsonObject> GetLibaray(winrt::hstring const& name);
	[[nodiscard]] winrt::Windows::Foundation::IAsyncAction RemoveLibrary(winrt::hstring const& name);
	[[nodiscard]] winrt::Windows::Foundation::IAsyncAction RemoveAllData();
}
