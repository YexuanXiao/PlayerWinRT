#pragma once

#include <pch.h>

// Win32Helper is responsible for all Win32 API interactions
namespace Win32Helper {
    /// <summary>
    /// Make app only has one instance
    /// call at entry point
    /// </summary>
    /// <param name="appname">size lesser than 16, zero ending</param>
    /// <param name="exitcode">exit code</param>
    void DisableMultiInstanceEntry(std::wstring_view const, UINT const);
    [[nodiscard]] HWND GetHandleFromWindow(winrt::Microsoft::UI::Xaml::Window const&);
    /// <summary>
    /// Make app only has one instance
    /// call at window_ initialized
    /// </summary>
    /// <param name="appname">size lesser than 16</param>
    /// <param name="handle">handle of window_</param>
    void DisableMultiInstanceWindow(winrt::Microsoft::UI::Xaml::Window const&, std::wstring_view const);
    [[nodiscard]] double GetScaleAdjustment(winrt::Microsoft::UI::Xaml::Window const&);
    LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
    void RegisterWindowMinSize(winrt::Microsoft::UI::Xaml::Window const&);
    void RegistCoreWindow(winrt::Windows::Foundation::IInspectable const&);
    [[nodiscard]] winrt::hstring GetMusicFolderPath();
}
