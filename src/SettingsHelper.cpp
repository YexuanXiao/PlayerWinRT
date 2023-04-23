#include "pch.h"
#include "SettingsHelper.h"

namespace SettingsHelper {

	namespace impl_ {
		constexpr std::wstring_view Theme_Key = L"Theme";
		constexpr std::wstring_view First_Key = L"First";
		winrt::Windows::Foundation::Collections::IPropertySet GetApplicationSettings() {
			return winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
		}
		void StoreTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme) {
			auto localSettings{ impl_::GetApplicationSettings() };
			auto value{ static_cast<int32_t>(theme) };
			localSettings.Insert(impl_::Theme_Key.data(), winrt::box_value<int32_t>(value));
		}
	}
	winrt::Microsoft::UI::Xaml::ElementTheme LoadTheme() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<int32_t>(localSettings.Lookup(impl_::Theme_Key.data()),0) };
		return winrt::Microsoft::UI::Xaml::ElementTheme{value};
	}
	// for settings page use
	void SetTheme(winrt::Microsoft::UI::Xaml::XamlRoot& element, winrt::Microsoft::UI::Xaml::ElementTheme theme) {
		element.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(theme);
		impl_::StoreTheme(theme);
	}
	// for init
	void SetTheme(winrt::Microsoft::UI::Xaml::FrameworkElement& element, winrt::Microsoft::UI::Xaml::ElementTheme theme) {
		element.RequestedTheme(theme);
	}
	// for other page
	void SetTheme(winrt::Microsoft::UI::Xaml::XamlRoot& element) {
		element.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(LoadTheme());
	}
	void SetTitleBarTheme(winrt::Microsoft::UI::Windowing::AppWindowTitleBar& titlebar, winrt::Microsoft::UI::Xaml::ElementTheme theme) {
		if (theme == winrt::Microsoft::UI::Xaml::ElementTheme::Default) [[likely]] {
			titlebar.BackgroundColor(nullptr);
			titlebar.ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
			titlebar.ButtonForegroundColor(nullptr);
			titlebar.ButtonInactiveBackgroundColor(nullptr);
			titlebar.ButtonInactiveForegroundColor(nullptr);
			titlebar.ButtonHoverBackgroundColor(nullptr);
			titlebar.ButtonHoverForegroundColor(nullptr);
			titlebar.ButtonPressedBackgroundColor(nullptr);
			titlebar.ButtonPressedForegroundColor(nullptr);
		}
		else if (theme == winrt::Microsoft::UI::Xaml::ElementTheme::Dark) {
			titlebar.BackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
			titlebar.ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
			titlebar.ButtonForegroundColor(winrt::Microsoft::UI::Colors::White());
			titlebar.ButtonInactiveBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
			titlebar.ButtonInactiveForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 96, 96, 96));
			titlebar.ButtonHoverBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 45, 45, 45));
			titlebar.ButtonHoverForegroundColor(winrt::Microsoft::UI::Colors::White());
			titlebar.ButtonPressedBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 41, 41, 41));
			titlebar.ButtonPressedForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 167, 167, 167));
		}
		else if (theme == winrt::Microsoft::UI::Xaml::ElementTheme::Light) {
			titlebar.BackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
			titlebar.ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
			titlebar.ButtonForegroundColor(winrt::Microsoft::UI::Colors::Black());
			titlebar.ButtonInactiveBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 243, 243, 243));
			titlebar.ButtonInactiveForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 144, 144, 144));
			titlebar.ButtonHoverBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 219, 219, 219));
			titlebar.ButtonHoverForegroundColor(winrt::Microsoft::UI::Colors::Black());
			titlebar.ButtonPressedBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 194, 194, 194));
			titlebar.ButtonPressedForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 95, 95, 95));
		}
	}
	bool CheckFirstUse() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto result{ localSettings.HasKey(impl_::First_Key.data()) };
		if (result) [[likely]] {
			return false;
		}
		else {
#ifndef _DEBUG
			localSettings.Insert(impl_::First_Key.data(), winrt::box_value<bool>(true));
#endif
			return true;
		}
	}
}
