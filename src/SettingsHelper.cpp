#include "pch.h"
#include "SettingsHelper.h"

namespace SettingsHelper {

	namespace impl_ {
		constexpr std::wstring_view Theme_Key = L"Theme";
		constexpr std::wstring_view First_Key = L"First";
		constexpr std::wstring_view Language_Key = L"Language";
		constexpr std::wstring_view Volume_Key = L"Volume";
		constexpr std::wstring_view Repeat_Key = L"Repeat";
		constexpr std::wstring_view Libraries_Key = L"Libraries";
		winrt::Windows::Foundation::Collections::IPropertySet GetApplicationSettings() {
			return winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
		}
		winrt::Windows::Storage::StorageFolder GetDataFolder() {
			return winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
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
		element.Content().try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(theme);
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
		auto value{ winrt::unbox_value_or<double>(localSettings.Lookup(impl_::Volume_Key.data()),100.) };
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
	winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> GetLibraries() {
		auto localSettings{ impl_::GetApplicationSettings() };
		auto value{ winrt::unbox_value_or<winrt::hstring>(localSettings.Lookup(impl_::Libraries_Key.data()),winrt::hstring{L"[]"})};
		auto libraries{ winrt::Windows::Data::Json::JsonArray::Parse(value) };
		// make container and data
		auto container{ std::vector<winrt::Data::Library>{} };
		container.reserve(libraries.Size());
		for (auto const& library : libraries) {
			auto info{ library.GetObjectW() };
			container.emplace_back(info.GetNamedString(L"Name"), info.GetNamedString(L"Protocol"), info.GetNamedString(L"Path"), info.GetNamedString(L"Icon"));
		}
		return winrt::single_threaded_observable_vector<winrt::Data::Library>(std::move(container));
	}
	winrt::Windows::Foundation::IAsyncAction StoreLibrary(winrt::Windows::Data::Json::JsonObject const& library) {
		auto localSettings{ impl_::GetApplicationSettings() };
		// first, insert library to local settings
		auto const name{ library.GetNamedString(L"Name") };
		auto file{ co_await impl_::GetDataFolder().CreateFileAsync(name,winrt::Windows::Storage::CreationCollisionOption::OpenIfExists) };
		winrt::Windows::Storage::FileIO::WriteTextAsync(file, library.GetNamedArray(L"List").ToString());
		// second, insert library name to libraries
		auto libraries{ winrt::Windows::Data::Json::JsonArray::Parse(winrt::unbox_value_or<winrt::hstring>(localSettings.Lookup(impl_::Libraries_Key.data()),winrt::hstring{L"[]"}))};
		auto libraryinfo{ winrt::Windows::Data::Json::JsonObject{} };
		libraryinfo.SetNamedValue(L"Name", library.GetNamedValue(L"Name"));
		libraryinfo.SetNamedValue(L"Path", library.GetNamedValue(L"Path"));
		libraryinfo.SetNamedValue(L"Protocol", library.GetNamedValue(L"Protocol"));
		libraryinfo.SetNamedValue(L"Icon", library.GetNamedValue(L"Icon"));
		libraries.Append(libraryinfo);
		localSettings.Insert(impl_::Libraries_Key.data(), winrt::box_value(libraries.ToString()));
	}
	winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Data::Json::JsonArray> GetLibrary(winrt::hstring const& name) {
		auto file{ co_await impl_::GetDataFolder().GetItemAsync(name)};
		co_return winrt::Windows::Data::Json::JsonArray::Parse(co_await winrt::Windows::Storage::FileIO::ReadTextAsync(file.try_as<winrt::Windows::Storage::StorageFile>()));
	}
	winrt::Windows::Foundation::IAsyncAction RemoveLibrary(winrt::hstring const& name) {
		auto localSettings{ impl_::GetApplicationSettings() };
		// first, remove library data
		auto file{ co_await impl_::GetDataFolder().GetItemAsync(name) };
		co_await file.try_as<winrt::Windows::Storage::StorageFile>().DeleteAsync();
		// second, remove library from libraries
		auto libraries{ winrt::Windows::Data::Json::JsonArray::Parse(winrt::unbox_value_or<winrt::hstring>(localSettings.Lookup(impl_::Libraries_Key.data()),winrt::hstring{L"[]"})) };
		auto index{ decltype(libraries.Size()){} };
		for (auto const& i : libraries) {
			auto library{ i.GetObjectW() };
			if (library.GetNamedString(L"Name") == name) {
				libraries.RemoveAt(index);
				break;
			}
			else {
				++index;
			};
		}
		localSettings.Insert(impl_::Libraries_Key.data(), winrt::box_value(libraries.ToString()));
	}
	winrt::Windows::Foundation::IAsyncAction RemoveAllData() {
		return winrt::Windows::Storage::ApplicationData::Current().ClearAsync();
	}
}
