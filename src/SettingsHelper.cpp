#include "pch.h"
#include "SettingsHelper.h"

namespace SettingsHelper {

	namespace impl_ {
		constexpr std::wstring_view Theme_Key = L"Theme";
		constexpr std::wstring_view First_Key = L"First";
		constexpr std::wstring_view Language_Key = L"Language";
		constexpr std::wstring_view Volume_Key = L"Volume";
		constexpr std::wstring_view Repeat_Key = L"Repeat";
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
	double GetVolume() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<double>(localSettings.Lookup(impl_::Volume_Key.data()),1.) };
		return value;
	}
	void SetVolume(double value) {
		auto localSettings{ impl_::GetApplicationSettings() };
		localSettings.Insert(impl_::Volume_Key.data(), winrt::box_value(value));
	}
	Repeat GetRepeat() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<int32_t>(localSettings.Lookup(impl_::Repeat_Key.data()),0) };
		return Repeat{value};
	}
	void SetRepeat(Repeat value) {
		auto localSettings{ impl_::GetApplicationSettings() };
		localSettings.Insert(impl_::Repeat_Key.data(), winrt::box_value(static_cast<int32_t>(value)));
	}
}
