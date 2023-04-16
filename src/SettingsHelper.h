#pragma once
#include "pch.h"

using namespace winrt;

namespace SettingsHelper {

	namespace impl_ {
		Windows::Foundation::Collections::IPropertySet GetApplicationSettings();
		void StoreTheme(Microsoft::UI::Xaml::ElementTheme theme);
	}
	Microsoft::UI::Xaml::ElementTheme LoadTheme();
	// for settings page use
	void SetTheme(Microsoft::UI::Xaml::XamlRoot& element, Microsoft::UI::Xaml::ElementTheme theme);
	// for init
	void SetTheme(Microsoft::UI::Xaml::FrameworkElement& element, Microsoft::UI::Xaml::ElementTheme theme);
	// for other page
	void SetTheme(Microsoft::UI::Xaml::XamlRoot& element);
	void SetTitleBarTheme(Microsoft::UI::Windowing::AppWindowTitleBar& titlebar, Microsoft::UI::Xaml::ElementTheme theme);
}
