// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

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

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Player::implementation
{
	Settings::Settings()
	{
		InitializeComponent();
	}
	void Settings::Color_Changed(IInspectable const& sender, SelectionChangedEventArgs const&)
	{
		auto radioButton{ sender.as<RadioButtons>().SelectedItem()
			.try_as<RadioButton>() };
		// if no item select, return
		if (radioButton == nullptr) return;

		auto tagName{ winrt::unbox_value<winrt::hstring>(radioButton.Tag()) };
		auto theme{ ElementTheme{ 0 } };

		if (tagName == L"dark") {
			theme = ElementTheme{ 2 };
		}
		else if (tagName == L"light") {
			theme = ElementTheme{ 1 };
		}
		auto xamlRoot{ XamlRoot() };
		SettingsHelper::SetTheme(xamlRoot,theme);
	}
}
