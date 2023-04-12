#pragma once

#include <pch.h>

#include <winrt/Windows.Storage.h>

using namespace winrt;

namespace SettingsHelper {

	namespace impl_ {
		inline constexpr std::wstring_view Theme_Key = L"Theme";

		inline Windows::Foundation::Collections::IPropertySet GetApplicationSettings() {
			return Windows::Storage::ApplicationData::Current().LocalSettings().Values();
		}
		inline void StoreTheme(Microsoft::UI::Xaml::ElementTheme theme) {
			auto localSettings{ impl_::GetApplicationSettings() };
			auto value{ static_cast<int32_t>(theme) };
			localSettings.Insert(impl_::Theme_Key.data(), winrt::box_value<int32_t>(value));
		}
	}
	inline Microsoft::UI::Xaml::ElementTheme LoadTheme() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<int32_t>(localSettings.Lookup(impl_::Theme_Key.data()),0) };
		return Microsoft::UI::Xaml::ElementTheme{value};
	}
	// for settings page use
	inline void SetTheme(Microsoft::UI::Xaml::XamlRoot& element, Microsoft::UI::Xaml::ElementTheme theme) {
		element.Content().as<Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(theme);
		impl_::StoreTheme(theme);
	}
	// for other page
	inline void SetTheme(Microsoft::UI::Xaml::XamlRoot& element) {
		element.Content().as<Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(LoadTheme());
	}
}