#include "pch.h"
#include "LibraryEditor.xaml.h"
#if __has_include("LibraryEditor.g.cpp")
#include "LibraryEditor.g.cpp"
#endif

#include <regex>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Storage.AccessCache.h>
#include "Win32Helper.h"
#include "SettingsHelper.h"
#include "Data.h"

namespace winrt::Player::implementation
{
    LibraryEditor::LibraryEditor(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> const& libraries) : libraries_(libraries)
    {
        InitializeComponent();

        Title(winrt::box_value(resource_.GetString(L"AddLibrary/Text")));
        PrimaryButtonText(resource_.GetString(L"Add"));
        CloseButtonText(resource_.GetString(L"Cancel"));

        // initialize icons, add click event
        for (auto icon : Icons().Items())
            icon.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Tapped({ this, &LibraryEditor::Icon_Select });

        // register add branch
        PrimaryButtonClick({ this, &LibraryEditor::PrimaryButton_Click });
    }

    LibraryEditor::LibraryEditor(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> const& libraries, winrt::hstring const& name, winrt::hstring const& protocol, winrt::hstring const& address, winrt::hstring const& icon) : libraries_(libraries)
    {
        InitializeComponent();

        Title(winrt::box_value(resource_.GetString(L"EditLibrary/Content")));
        PrimaryButtonText(resource_.GetString(L"Save"));
        CloseButtonText(resource_.GetString(L"Cancel"));

        // initialize icons, add click event
        for (auto item : Icons().Items())
            item.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Tapped({ this, &LibraryEditor::Icon_Select });

        Library().Text(name);
        Protocol().Text(protocol);
        Address().Text(address);
        if (!icon.empty())
            // edit branch
            Icon().Glyph(icon);
        else
            // welcome branch
            // register add event
            PrimaryButtonClick({ this, &LibraryEditor::PrimaryButton_Click });
        ;
    }

    void LibraryEditor::Icon_Select(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        Icon().Glyph(sender.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Icon().try_as<winrt::Microsoft::UI::Xaml::Controls::FontIcon>().Glyph());
    }

    void LibraryEditor::ProtocolMenu_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto tag{ sender.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Tag().try_as<hstring>() };

        if (tag == L"local") [[likely]]
        {
            auto resource{ winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
            Protocol().Text(resource.GetString(L"Local/Text"));
            Address().Text(L"X:\\");
            Icon().Glyph(L"\uE770");
            SelectButton().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
        }
        else if (tag == L"ftp")
        {
            Protocol().Text(L"FTP");
            Address().Text(L"ftp://");
            Icon().Glyph(L"\uE839");
            SelectButton().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }
        else if (tag == L"samba")
        {
            Protocol().Text(L"Samba");
            Address().Text(L"smb://");
            Icon().Glyph(L"\uEC27");
            SelectButton().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }
        else if (tag == L"webdav")
        {
            Protocol().Text(L"WebDAV");
            Address().Text(L"http://");
            Icon().Glyph(L"\uE968");
            SelectButton().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        }
    }

    winrt::Windows::Foundation::IAsyncAction LibraryEditor::PrimaryButton_Click(winrt::Microsoft::UI::Xaml::Controls::ContentDialog const&, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const& args)
    {
        // Cancel = true keep Dialog show after click
        args.Cancel(true);
        if (!CheckInfoLegal())
            co_return;

        Progress().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
        Editor().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
        PrimaryButtonText(winrt::hstring{});
        // capture ui thread
        auto ui_thread{ winrt::apartment_context{} };

        auto info{ winrt::Data::Library{ Library().Text(), winrt::unbox_value<winrt::hstring>(Protocol().Tag()), Address().Text(), Icon().Glyph() } };
        auto library{ winrt::Windows::Data::Json::JsonObject{ nullptr } };
        if (info.protocol == L"local") [[likely]]
        {
            auto task{ ::Data::GetLibraryFromFolderPath(info.name, info.protocol, info.address, info.icon) };
            // register cancel event, must capture by value, otherwise hold a dangling reference
            CloseButtonClick([task](winrt::Microsoft::UI::Xaml::Controls::ContentDialog const&, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const&) { task.Cancel(); });
            // switch to other thread
            co_await winrt::resume_background();
            library = co_await task;
        }
        else
        {
            // todo: other protocol
        }

        co_await SettingsHelper::StoreLibrary(library);

        libraries_.Append(info);
        // switch back to ui thread
        co_await ui_thread;
        Hide();
    }

    winrt::Windows::Foundation::IAsyncAction LibraryEditor::SelectButton_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& args)
    {
        args.Handled(true);
        auto picker{ winrt::Windows::Storage::Pickers::FolderPicker{} };
        picker.ViewMode(winrt::Windows::Storage::Pickers::PickerViewMode::List);
        picker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::ComputerFolder);
        Win32Helper::InitializeCoreWindow(picker);
        auto const& folder{ co_await picker.PickSingleFolderAsync() };
        if (folder != nullptr) [[likely]]
        {
            winrt::Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList().Add(folder);
            Address().Text(folder.Path());
            auto library{ Library() };
            if (library.Text().size()) [[unlikely]]
                co_return;
            auto path{ std::wstring_view{ folder.Path() } };
            auto position{ path.find_last_of(L'\\') };
            auto name{ std::wstring_view{ path.begin() + position + 1, path.end() } };
            library.Text(name);
        }
    }

    bool LibraryEditor::CheckInfoLegal()
    {
        auto name{ Library().Text() };
        auto protocol{ winrt::unbox_value<winrt::hstring>(Protocol().Tag()) };
        auto address{ Address().Text() };
        if (name.empty() || protocol.empty() || address.empty())
        {
            goto Illegal;
        }
        if (protocol == L"local")
        {
            goto Legal;
        }
        if (protocol == L"ftp")
        {
            if (std::regex_match(std::wstring{ address }, std::wregex{ LR"(^ftp://(?:\w+:\w+@)?\w+(?:\.\w+)+(/.*)?$)" }))
                goto Legal;
            else
                goto Illegal;
        }
        if (protocol == L"webdav")
        {
            if (std::regex_match(std::wstring{ address }, std::wregex{ LR"(^https?://\w+(?:\.\w+)+(/.*)?$)" }))
                goto Legal;
            else
                goto Illegal;
        }
    Legal:
        ErrorInfo().IsOpen(false);
        return true;
    Illegal:
        ErrorInfo().IsOpen(true);
        return false;
    }
}
