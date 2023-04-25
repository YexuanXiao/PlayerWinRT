#include "pch.h"
#include "Settings.xaml.h"
#if __has_include("Settings.g.cpp")
#include "Settings.g.cpp"
#endif

#include <SettingsHelper.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;

using namespace Windows::Globalization;

namespace winrt::Player::implementation
{
	Settings::Settings()
	{
		InitializeComponent();
		auto items{ Languages().Items() };

		// add default lang item
		{
			auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
			auto item{ MenuFlyoutItem{} };
			item.Text(resourceLoader.GetString(L"Default/Content"));
			item.Tag(winrt::box_value(hstring{ L"default" }));
			item.Click(&Settings::Language_Selected);
			items.Append(item);
		}
		// add manifest langs
		for (auto lang : ApplicationLanguages::ManifestLanguages()) {
			auto item{ MenuFlyoutItem{} };
			item.Text(GetLangTagName(lang));
			item.Tag(winrt::box_value(lang));
			item.Click(&Settings::Language_Selected);
			items.Append(item);
		}
	}

	void Settings::Theme_Changed(IInspectable const& sender, SelectionChangedEventArgs const&)
	{
		auto radioButton{ sender.as<RadioButtons>().SelectedItem().try_as<RadioButton>() };
		// if no item select, return
		if (radioButton == nullptr) [[likely]] return;

		auto tagName{ winrt::unbox_value<winrt::hstring>(radioButton.Tag()) };
		auto theme{ ElementTheme::Default };

		if (tagName == L"dark") [[likely]] {
			theme = ElementTheme::Dark;
		}
		else if (tagName == L"light") {
			theme = ElementTheme::Light;
		}
		SettingsHelper::SetTheme(XamlRoot(), theme);
	}

	void Settings::Theme_Loaded(IInspectable const& sender, RoutedEventArgs const&)
	{
		auto radioButtons{ sender.as<RadioButtons>() };
		auto pre{ SettingsHelper::LoadTheme() };
		radioButtons.SelectedIndex(static_cast<int32_t>(pre));
	}

	void Settings::Language_Selected(IInspectable const& sender, RoutedEventArgs const&)
	{
		auto lang{ winrt::unbox_value<hstring>(sender.as<MenuFlyoutItem>().Tag()) };
		if (lang == L"default") [[unlikely]] {
			ApplicationLanguages::PrimaryLanguageOverride(hstring{});
			return;
		}
			// always persistence 
		ApplicationLanguages::PrimaryLanguageOverride(lang);
	}

	hstring Settings::GetLangTagName(hstring const& tag) {
		if (tag.starts_with(L"en")) {
			return { L"English" };
		}
		else if (tag.starts_with(L"zh")) [[likely]] {
			return { L"Chinese" }; 
		}
		else {
			assert(false);
			return { L"Unknown Language" };
		}
	}
}