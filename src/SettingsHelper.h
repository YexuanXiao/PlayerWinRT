#pragma once
#include "pch.h"

using namespace winrt;

namespace SettingsHelper {

	namespace impl_ {
		Windows::Foundation::Collections::IPropertySet GetApplicationSettings();
		void StoreTheme(Microsoft::UI::Xaml::ElementTheme);
	}
	Microsoft::UI::Xaml::ElementTheme LoadTheme();
	// for settings page use
	void SetTheme(Microsoft::UI::Xaml::XamlRoot, Microsoft::UI::Xaml::ElementTheme);
	// for init
	void SetTheme(Microsoft::UI::Xaml::FrameworkElement&, Microsoft::UI::Xaml::ElementTheme);
	bool CheckFirstUse();
	double GetVolume();
	void SetVolume(double);
	enum class Repeat:int32_t {
		None,One,All
	};
	Repeat GetRepeat();
	void SetRepeat(Repeat);
}
