#include "pch.h"
#include "SettingsHelper.h"

namespace SettingsHelper {

	namespace impl_ {
		constexpr std::wstring_view Theme_Key = L"Theme";
		constexpr std::wstring_view First_Key = L"First";
		constexpr std::wstring_view Language_Key = L"Language";
		winrt::Windows::Foundation::Collections::IPropertySet GetApplicationSettings() {
			return winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
		}
		void StoreTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme) {
			auto localSettings{ impl_::GetApplicationSettings() };
			auto value{ static_cast<int32_t>(theme) };
			localSettings.Insert(impl_::Theme_Key.data(), winrt::box_value(value));
		}
	}
	winrt::Microsoft::UI::Xaml::ElementTheme LoadTheme() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<int32_t>(localSettings.Lookup(impl_::Theme_Key.data()),0) };
		return winrt::Microsoft::UI::Xaml::ElementTheme{value};
	}
	// for settings page use
	void SetTheme(winrt::Microsoft::UI::Xaml::XamlRoot element, winrt::Microsoft::UI::Xaml::ElementTheme theme) {
		element.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(theme);
		impl_::StoreTheme(theme);
	}
	// for init
	void SetTheme(winrt::Microsoft::UI::Xaml::FrameworkElement& element, winrt::Microsoft::UI::Xaml::ElementTheme theme) {
		element.RequestedTheme(theme);
	}

	bool CheckFirstUse() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto result{ localSettings.HasKey(impl_::First_Key.data()) };
		if (result) [[likely]] {
			return false;
		}
		else {
#ifndef _DEBUG
			localSettings.Insert(impl_::First_Key.data(), winrt::box_value(true));
#endif
			return true;
		}
	}
	winrt::hstring GetLanguage() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<winrt::hstring>(localSettings.Lookup(impl_::Language_Key.data()),winrt::hstring{}) };
		return value;
	}
	void SetLanguage(winrt::hstring const& lang) {
		auto localSettings{ impl_::GetApplicationSettings() };
		localSettings.Insert(impl_::Language_Key.data(), winrt::box_value(lang));
	}
}
