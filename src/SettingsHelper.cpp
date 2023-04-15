#include "pch.h"
#include "SettingsHelper.h"

using namespace winrt;

namespace SettingsHelper {

	namespace impl_ {
		constexpr std::wstring_view Theme_Key = L"Theme";
		Windows::Foundation::Collections::IPropertySet GetApplicationSettings() {
			return Windows::Storage::ApplicationData::Current().LocalSettings().Values();
		}
		void StoreTheme(Microsoft::UI::Xaml::ElementTheme theme) {
			auto localSettings{ impl_::GetApplicationSettings() };
			auto value{ static_cast<int32_t>(theme) };
			localSettings.Insert(impl_::Theme_Key.data(), winrt::box_value<int32_t>(value));
		}
	}
	Microsoft::UI::Xaml::ElementTheme LoadTheme() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<int32_t>(localSettings.Lookup(impl_::Theme_Key.data()),0) };
		return Microsoft::UI::Xaml::ElementTheme{value};
	}
	// for settings page use
	void SetTheme(Microsoft::UI::Xaml::XamlRoot& element, Microsoft::UI::Xaml::ElementTheme theme) {
		element.Content().as<Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(theme);
		impl_::StoreTheme(theme);
	}
	// for init
	void SetTheme(Microsoft::UI::Xaml::FrameworkElement& element, Microsoft::UI::Xaml::ElementTheme theme) {
		element.RequestedTheme(theme);
	}
	// for other page
	void SetTheme(Microsoft::UI::Xaml::XamlRoot& element) {
		element.Content().as<Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(LoadTheme());
	}
	void SetTitleBarTheme(Microsoft::UI::Windowing::AppWindowTitleBar& titlebar, Microsoft::UI::Xaml::ElementTheme theme) {
		if (theme == Microsoft::UI::Xaml::ElementTheme::Default) [[likely]] {
			titlebar.BackgroundColor(nullptr);
			titlebar.ButtonBackgroundColor(Microsoft::UI::Colors::Transparent());
			titlebar.ButtonForegroundColor(nullptr);
			titlebar.ButtonInactiveBackgroundColor(nullptr);
			titlebar.ButtonInactiveForegroundColor(nullptr);
			titlebar.ButtonHoverBackgroundColor(nullptr);
			titlebar.ButtonHoverForegroundColor(nullptr);
			titlebar.ButtonPressedBackgroundColor(nullptr);
			titlebar.ButtonPressedForegroundColor(nullptr);
		}
		else if (theme == Microsoft::UI::Xaml::ElementTheme::Dark) {
			titlebar.BackgroundColor(Microsoft::UI::Colors::Transparent());
			titlebar.ButtonBackgroundColor(Microsoft::UI::Colors::Transparent());
			titlebar.ButtonForegroundColor(Microsoft::UI::Colors::White());
			titlebar.ButtonInactiveBackgroundColor(Microsoft::UI::Colors::Transparent());
			titlebar.ButtonInactiveForegroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 96, 96, 96));
			titlebar.ButtonHoverBackgroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 45, 45, 45));
			titlebar.ButtonHoverForegroundColor(Microsoft::UI::Colors::White());
			titlebar.ButtonPressedBackgroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 41, 41, 41));
			titlebar.ButtonPressedForegroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 167, 167, 167));
		}
		else if (theme == Microsoft::UI::Xaml::ElementTheme::Light) {
			titlebar.BackgroundColor(Microsoft::UI::Colors::Transparent());
			titlebar.ButtonBackgroundColor(Microsoft::UI::Colors::Transparent());
			titlebar.ButtonForegroundColor(Microsoft::UI::Colors::Black());
			titlebar.ButtonInactiveBackgroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 243, 243, 243));
			titlebar.ButtonInactiveForegroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 144, 144, 144));
			titlebar.ButtonHoverBackgroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 219, 219, 219));
			titlebar.ButtonHoverForegroundColor(Microsoft::UI::Colors::Black());
			titlebar.ButtonPressedBackgroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 194, 194, 194));
			titlebar.ButtonPressedForegroundColor(Microsoft::UI::ColorHelper::FromArgb(255, 95, 95, 95));
		}
	}
}