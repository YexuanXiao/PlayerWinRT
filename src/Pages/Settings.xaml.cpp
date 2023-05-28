#include "pch.h"
#include "Settings.xaml.h"
#if __has_include("Settings.g.cpp")
#include "Settings.g.cpp"
#endif

#include <SettingsHelper.h>

#include <winrt/Windows.Globalization.h>

namespace winrt::Player::implementation
{
	Settings::Settings()
	{
		InitializeComponent();
		auto items{ Languages().Items() };

		// add default lang item
		{
			auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
			auto item{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{}};
			item.Text(resourceLoader.GetString(L"Default/Content"));
			item.Tag(winrt::box_value(hstring{ L"default" }));
			item.Tapped(&Settings::Language_Selected);
			items.Append(item);
		}
		// add manifest langs
		for (auto lang : winrt::Windows::Globalization::ApplicationLanguages::ManifestLanguages()) [[likely]] {
			auto item{ winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem{} };
			item.Text(GetLangTagName(lang));
			item.Tag(winrt::box_value(lang));
			item.Tapped(&Settings::Language_Selected);
			items.Append(item);
		}

		// first use
		if (!SettingsHelper::CheckFirstUse()) [[likely]]
			return;

		for (auto item : ContentGrid().Children()) [[likely]] {
			auto expander{ item.try_as<winrt::Microsoft::UI::Xaml::Controls::Expander>() };
			if (expander != nullptr)
				expander.IsExpanded(true);
		}
		
	}

	void Settings::Theme_Changed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&)
	{
		auto radioButton{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::RadioButtons>().SelectedItem().try_as<winrt::Microsoft::UI::Xaml::Controls::RadioButton>() };
		// if no item select, return
		if (radioButton == nullptr) [[likely]] return;

		auto tagName{ winrt::unbox_value<winrt::hstring>(radioButton.Tag()) };
		auto theme{ winrt::Microsoft::UI::Xaml::ElementTheme::Default };

		if (tagName == L"dark") [[likely]]
			theme = winrt::Microsoft::UI::Xaml::ElementTheme::Dark;
		else if (tagName == L"light")
			theme = winrt::Microsoft::UI::Xaml::ElementTheme::Light;

		SettingsHelper::SetTheme(XamlRoot(), theme);
	}

	void Settings::Theme_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto radioButtons{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::RadioButtons>() };
		auto pre{ SettingsHelper::LoadTheme() };
		radioButtons.SelectedIndex(static_cast<int32_t>(pre));
	}

	void Settings::Language_Selected(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
	{
		auto lang{ winrt::unbox_value<hstring>(sender.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Tag()) };
		if (lang == L"default") [[unlikely]]
			winrt::Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride(hstring{}); 
		else
		// always persistence
			winrt::Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride(lang);
	}

	hstring Settings::GetLangTagName(hstring const& tag) {
		if (tag.starts_with(L"en"))
			return { L"English" };
		else if (tag.starts_with(L"zh")) [[likely]]
			return { L"Chinese" };

		// else
		assert(false);
		std::unreachable();
	}
	winrt::Windows::Foundation::IAsyncAction Settings::Reset_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
	{
		args.Handled(true);
		auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
		auto theme{ ActualTheme() };
		auto dialog{ winrt::Microsoft::UI::Xaml::Controls::ContentDialog{} };
		dialog.XamlRoot(XamlRoot());
		dialog.Title(winrt::box_value(L"Reset Application"));
		dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
		dialog.DefaultButton(winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Close);
		dialog.PrimaryButtonText(L"Reset");
		auto content{ winrt::Microsoft::UI::Xaml::Controls::Grid{} };
		auto rows{ content.RowDefinitions() };
		auto def1{ winrt::Microsoft::UI::Xaml::Controls::RowDefinition{} };
		def1.Height(winrt::Microsoft::UI::Xaml::GridLengthHelper::Auto());
		auto def2{ winrt::Microsoft::UI::Xaml::Controls::RowDefinition{} };
		def2.Height(winrt::Microsoft::UI::Xaml::GridLengthHelper::Auto());
		rows.Append(def1);
		rows.Append(def2);
		auto icon{ winrt::Microsoft::UI::Xaml::Controls::FontIcon{} };
		auto font{ winrt::Microsoft::UI::Xaml::Application::Current().Resources().Lookup(winrt::box_value(L"IconFontFamily")).try_as<winrt::Microsoft::UI::Xaml::Media::FontFamily>() };
		icon.FontFamily(font);
		icon.Glyph(L"\uE7BA");
		icon.FontSize(50.);
		icon.Margin(winrt::Microsoft::UI::Xaml::ThicknessHelper::FromUniformLength(30.));
		icon.MaxWidth(300.);
		auto text{ winrt::Microsoft::UI::Xaml::Controls::TextBlock{} };
		text.Text(L"Are you sure you want to delete all data of this app? This operation is irreversible!");
		text.TextAlignment(winrt::Microsoft::UI::Xaml::TextAlignment::Center);
		text.TextWrapping(winrt::Microsoft::UI::Xaml::TextWrapping::Wrap);
		text.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Center);
		winrt::Microsoft::UI::Xaml::Controls::Grid::SetRow(icon, 0);
		winrt::Microsoft::UI::Xaml::Controls::Grid::SetRow(text, 1);
		auto children{ content.Children() };
		children.Append(icon);
		children.Append(text);
		dialog.Content(content);
		dialog.RequestedTheme(theme);
		auto result{ co_await dialog.ShowAsync() };
		if (result != winrt::Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary) co_return;
		co_await SettingsHelper::RemoveAllData();
		winrt::Microsoft::UI::Xaml::Application::Current().Exit();
	}
}