#pragma once

#include "LibraryEditor.g.h"
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.ApplicationModel.Resources.h>

namespace winrt::Player::implementation
{
    struct LibraryEditor: LibraryEditorT<LibraryEditor>
    {
        LibraryEditor(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> const&);
        LibraryEditor(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> const&, winrt::hstring const&, winrt::hstring const&, winrt::hstring const&, winrt::hstring const&);

        void ProtocolMenu_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        winrt::Windows::Foundation::IAsyncAction PrimaryButton_Click(winrt::Microsoft::UI::Xaml::Controls::ContentDialog const&, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const&);
        winrt::Windows::Foundation::IAsyncAction SelectButton_Tapped(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);

    private:
        bool CheckInfoLegal();
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceLoader resource_{};
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Data::Library> libraries_{ nullptr };
        void Icon_Select(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
    };
}

namespace winrt::Player::factory_implementation
{
    struct LibraryEditor: LibraryEditorT<LibraryEditor, implementation::LibraryEditor>
    {
    };
}
