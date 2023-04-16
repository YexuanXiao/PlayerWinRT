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

namespace winrt::Player::implementation
{
	Settings::Settings()
	{
		InitializeComponent();
	}
	void Settings::Theme_Changed(IInspectable const& sender, SelectionChangedEventArgs const&)
	{
		auto radioButton{ sender.as<RadioButtons>().SelectedItem().try_as<RadioButton>() };
		// if no item select, return
		if (radioButton == nullptr) return;

		auto tagName{ winrt::unbox_value<winrt::hstring>(radioButton.Tag()) };
		auto theme{ ElementTheme::Default };

		if (tagName == L"dark") {
			theme = ElementTheme::Dark;
		}
		else if (tagName == L"light") {
			theme = ElementTheme::Light;
		}
		auto xamlRoot{ XamlRoot() };
		SettingsHelper::SetTheme(xamlRoot, theme);
	}

	void Settings::Theme_Loaded(IInspectable const& sender, RoutedEventArgs const&)
	{
		auto radioButtons{ sender.as<RadioButtons>() };
		auto pre{ SettingsHelper::LoadTheme() };
		radioButtons.SelectedIndex(static_cast<int32_t>(pre));
	}
}
