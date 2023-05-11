#include "pch.h"
#include "EditLibrary.xaml.h"
#if __has_include("EditLibrary.g.cpp")
#include "EditLibrary.g.cpp"
#endif

#include "Progress.xaml.h"
#include "Win32Helper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

using namespace Windows::Storage::Pickers;

using namespace Windows::Storage::AccessCache;

using namespace Windows::Foundation;

namespace winrt::Player::implementation
{
    EditLibrary::EditLibrary()
    {
        InitializeComponent();
        // initialize icons, add click event
        for (auto i : Icons().Items()) {
            i.as<MenuFlyoutItem>().Tapped({ this,&EditLibrary::Icon_Select });
        }
    }
    EditLibrary::EditLibrary(hstring const& name, hstring const& protocol, hstring const& address, hstring const& icon) {
        InitializeComponent();
        Library().Text(name);
        Protocol().Text(protocol);
        Address().Text(address);
        Icon().Glyph(icon);
    }
    void EditLibrary::ProtocolMenu_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto tag{ sender.as<MenuFlyoutItem>().Tag().as<hstring>() };

        if (tag == L"local") [[likely]] {
            auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
            Protocol().Text(resourceLoader.GetString(L"Local/Text"));
            Address().Text(L"X:\\");
            Icon().Glyph(L"\uE770");
            SelectButton().Visibility(Visibility::Visible);
        }
        else if (tag == L"ftp") {
            Protocol().Text(L"FTP");
            Address().Text(L"ftp://");
            Icon().Glyph(L"\uE839");
            SelectButton().Visibility(Visibility::Collapsed);
        }
        else if (tag == L"samba") {
            Protocol().Text(L"Samba");
            Address().Text(L"smb://");
            Icon().Glyph(L"\uEC27");
            SelectButton().Visibility(Visibility::Collapsed);
        }
        else if (tag == L"webdav") {
            Protocol().Text(L"WebDAV");
            Address().Text(L"http://");
            Icon().Glyph(L"\uE968");
            SelectButton().Visibility(Visibility::Collapsed);
        }
    }
    IAsyncAction EditLibrary::SelectButton_Tapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        auto picker{ FolderPicker{} };
        picker.ViewMode(PickerViewMode::List);
        picker.SuggestedStartLocation(PickerLocationId::ComputerFolder);
        Win32Helper::RegistCoreWindow(picker);
        auto const& folder{ co_await picker.PickSingleFolderAsync() };
        if (folder != nullptr) [[likely]] {
            StorageApplicationPermissions::FutureAccessList().Add(folder);
            Address().Text(folder.Path());
        }
    }
    void EditLibrary::Icon_Select(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&)
    {
        Icon().Glyph(sender.as<MenuFlyoutItem>().Icon().as<FontIcon>().Glyph());
    }
    winrt::Data::Library EditLibrary::GetResult() {
        return { Library().Text(), winrt::unbox_value<hstring>(Protocol().Tag()), Address().Text(),Icon().Glyph()};
    }
}