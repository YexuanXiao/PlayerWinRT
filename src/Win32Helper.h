#pragma once

#include <pch.h>
#include <ShellScalingApi.h>
#include <Microsoft.UI.Xaml.Window.h>

namespace Win32Helper {
    /// <summary>
    /// Make app only has one instance
    /// call at entry point
    /// </summary>
    /// <param name="appname">size lesser than 16, zero ending</param>
    /// <param name="exitcode">exit code</param>
    void DisableMultiInstanceEntry(std::wstring_view const appname, UINT const exitcode);
    /// warp ::GetDpiForMonitor
    UINT GetDpiXForMonitor(HMONITOR hmonitor);
    HWND GetHandleFromWindow(winrt::Microsoft::UI::Xaml::Window const& window);
    winrt::Microsoft::UI::WindowId GetWindowIdFromWindow(winrt::Microsoft::UI::Xaml::Window const& window);
    /// <summary>
    /// Make app only has one instance
    /// call at window_ initialized
    /// </summary>
    /// <param name="appname">size lesser than 16</param>
    /// <param name="handle">handle of window_</param>
    void DisableMultiInstanceWindow(winrt::Microsoft::UI::Xaml::Window const& window, std::wstring_view const appname);
    double GetScaleAdjustment(winrt::Microsoft::UI::Xaml::Window const& window);
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void RegisterWindowMinSize(winrt::Microsoft::UI::Xaml::Window const& window);
}
