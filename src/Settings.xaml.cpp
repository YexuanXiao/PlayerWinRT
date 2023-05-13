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
			item.Tapped(&Settings::Language_Selected);
			items.Append(item);
		}
		// add manifest langs
		for (auto lang : ApplicationLanguages::ManifestLanguages()) {
			auto item{ MenuFlyoutItem{} };
			item.Text(GetLangTagName(lang));
			item.Tag(winrt::box_value(lang));
			item.Tapped(&Settings::Language_Selected);
			items.Append(item);
		}

		// first use
		if (SettingsHelper::CheckFirstUse()) {
			for (auto item : ContentGrid().Children()) {
				auto expander{ item.try_as<Expander>() };
				if(expander != nullptr){
					expander.IsExpanded(true);
				}
			}
		}
	}

	void Settings::Theme_Changed(IInspectable const& sender, SelectionChangedEventArgs const&)
	{
		auto radioButton{ sender.try_as<RadioButtons>().SelectedItem().try_as<RadioButton>() };
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
		auto radioButtons{ sender.try_as<RadioButtons>() };
		auto pre{ SettingsHelper::LoadTheme() };
		radioButtons.SelectedIndex(static_cast<int32_t>(pre));
	}

	void Settings::Language_Selected(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
	{
		auto lang{ winrt::unbox_value<hstring>(sender.try_as<MenuFlyoutItem>().Tag()) };
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
	winrt::Windows::Foundation::IAsyncAction Settings::Reset_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
	{
		auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
		auto theme{ ActualTheme() };
		auto dialog{ ContentDialog{} };
		dialog.XamlRoot(XamlRoot());
		dialog.Title(winrt::box_value(L"Reset Application"));
		dialog.CloseButtonText(resourceLoader.GetString(L"Cancel"));
		dialog.DefaultButton(ContentDialogButton::Close);
		dialog.PrimaryButtonText(L"Reset");
		auto content{ Grid{} };
		auto rows{ content.RowDefinitions() };
		auto def1{ RowDefinition{} };
		def1.Height(GridLengthHelper::Auto());
		auto def2{ RowDefinition{} };
		def2.Height(GridLengthHelper::Auto());
		rows.Append(def1);
		rows.Append(def2);
		auto icon{ FontIcon{} };
		icon.Glyph(L"\uE7BA");
		icon.FontSize(50.);
		icon.Margin(ThicknessHelper::FromUniformLength(30.));
		auto text{ TextBlock{} };
		text.Text(L"Are you sure you want to delete all data of this app? This operation is irreversible!");
		text.TextWrapping(TextWrapping::Wrap);
		text.HorizontalAlignment(HorizontalAlignment::Center);
		Grid::SetRow(icon, 0);
		Grid::SetRow(text, 1);
		auto children{ content.Children() };
		children.Append(icon);
		children.Append(text);
		dialog.Content(content);
		dialog.RequestedTheme(theme);
		auto result{ co_await dialog.ShowAsync() };
		if (result != ContentDialogResult::Primary) co_return;
		co_await SettingsHelper::RemoveAllData();
		Application::Current().Exit();
	}
}