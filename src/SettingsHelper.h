#pragma once
#include "pch.h"
#include "winrt/Data.h"

namespace SettingsHelper {

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
	winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> GetLibraries();
	[[nodiscard]] winrt::Windows::Foundation::IAsyncAction StoreLibrary(winrt::Windows::Data::Json::JsonObject const& library);
	[[nodiscard]] winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::JsonArray> GetLibrary(winrt::hstring const& name);
	[[nodiscard]] winrt::Windows::Foundation::IAsyncAction RemoveLibrary(winrt::hstring const& name);
	[[nodiscard]] winrt::Windows::Foundation::IAsyncAction RemoveAllData();
}
