#include "pch.h"
#include "EditLibrary.xaml.h"
#if __has_include("EditLibrary.g.cpp")
#include "EditLibrary.g.cpp"
#endif

#include <shobjidl_core.h>

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
        for (auto i : Icons().Items()) {
            i.as<MenuFlyoutItem>().Click({ this,&EditLibrary::Icon_Select });
        }
    }
    EditLibrary::EditLibrary(hstring const& name, hstring const& protocol, hstring const& address, hstring const& icon) {
        InitializeComponent();
        Library().Text(name);
        Protocol().Text(protocol);
        Address().Text(address);
        Icon().Glyph(icon);
    }
    void EditLibrary::ProtocolMenu_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto tag{ sender.as<MenuFlyoutItem>().Tag().as<hstring>() };

        if (tag == L"local") [[likely]] {
            auto resourceLoader{ Microsoft::Windows::ApplicationModel::Resources::ResourceLoader{} };
            SelectButton().Visibility(Visibility::Visible);
            Protocol().Text(resourceLoader.GetString(L"Local/Text"));
            Address().Text(L"X:\\");
            Icon().Glyph(L"\uE770");
            return;
        }
        else if (tag == L"ftp") {
            Protocol().Text(L"FTP");
            Address().Text(L"ftp://");
            Icon().Glyph(L"\uE839");
        }
        else if (tag == L"samba") {
            Protocol().Text(L"Samba");
            Address().Text(L"smb://");
            Icon().Glyph(L"\uEC27");
        }
        else if (tag == L"webdav") {
            Protocol().Text(L"WebDAV");
            Address().Text(L"http://");
            Icon().Glyph(L"\uE968");
        }
        SelectButton().Visibility(Visibility::Collapsed);
    }
    IAsyncAction EditLibrary::SelectButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        auto picker{ FolderPicker{} };
        picker.ViewMode(PickerViewMode::List);
        picker.SuggestedStartLocation(PickerLocationId::ComputerFolder);
        picker.as<IInitializeWithWindow>()->Initialize(GetActiveWindow());
        auto folder{ co_await picker.PickSingleFolderAsync() };
        if (folder != nullptr) [[likely]] {
            StorageApplicationPermissions::FutureAccessList().Add(folder);
            Address().Text(folder.Path());
        }
    }
    void EditLibrary::Icon_Select(Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        auto icon{ sender.as<MenuFlyoutItem>().Icon().as<FontIcon>().Glyph() };
        Icon().Glyph(icon);
    }
    EditLibrary::Result EditLibrary::GetResult() {
        return { Library().Text(), winrt::unbox_value<hstring>(Protocol().Tag()), Address().Text(),Icon().Glyph()};
    }
}