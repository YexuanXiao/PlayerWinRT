#pragma once
#include "pch.h"

namespace SettingsHelper {

	namespace impl_ {
		winrt::Windows::Foundation::Collections::IPropertySet GetApplicationSettings();
		void StoreTheme(winrt::Microsoft::UI::Xaml::ElementTheme);
	}
	winrt::Microsoft::UI::Xaml::ElementTheme LoadTheme();
	// for settings page use
	void SetTheme(winrt::Microsoft::UI::Xaml::XamlRoot, winrt::Microsoft::UI::Xaml::ElementTheme);
	// for init
	void SetTheme(winrt::Microsoft::UI::Xaml::FrameworkElement&, winrt::Microsoft::UI::Xaml::ElementTheme);
	bool CheckFirstUse();
	double GetVolume();
	void SetVolume(double);
	enum class Repeat:int32_t {
		None,One,All
	};
	Repeat GetRepeat();
	void SetRepeat(Repeat);
	winrt::Windows::Data::Json::JsonArray GetLibraries();
	void StoreLibrary(winrt::Windows::Data::Json::JsonObject const& library);
	winrt::Windows::Data::Json::JsonObject GetLibaray(winrt::hstring const& name);
	void RemoveLibrary(winrt::hstring const& name);
}
