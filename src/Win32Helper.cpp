#include "pch.h"
#include "Win32Helper.h"
#include <Microsoft.UI.Xaml.Window.h>
#include <shlobj.h>
#include <shobjidl_core.h>

namespace Win32Helper
{
    /// <summary>
    /// Make app only has one instance
    /// call at entry point
    /// </summary>
    /// <param name="appname">size lesser than 16, zero ending</param>
    /// <param name="exitcode">exit code</param>
    void DisableMultiInstanceEntry(std::wstring_view const appname, UINT const exitcode)
    {
        auto constexpr max_size{ 16 };
        assert(appname.size() < max_size);
        // use user's temp folder pathname + appname as part of mutex name
        // name buffer, initialized to be zero
        auto name{ std::to_array<wchar_t, (MAX_PATH + 2) + max_size>({}) };
        // get tmp folder path, not guaranteed availablity
        auto const length{ ::GetTempPathW(static_cast<DWORD>(name.size()), &name[0]) };
        // append appname to path
        std::memcpy(&name[length], appname.data(), appname.size() * sizeof(wchar_t));
        // mutex name can't contain '\'
        std::replace(&name[0], &name[length], '\\', '/');
        // if mutex is held by another instance, NO NEED TO USE GetLastError
        if (!::CreateMutexExW(nullptr, &name[0], CREATE_MUTEX_INITIAL_OWNER, NULL)) [[likely]]
        {
            // enumerate all direct child windows of desktop
            for (auto previous{ ::FindWindowExW(nullptr, nullptr, nullptr, nullptr) };
                 // return null if at the end
                 previous != nullptr;
                 // pass the previous as the 2nd argument to get next window_handle
                 previous = ::FindWindowExW(nullptr, previous, nullptr, nullptr)) [[likely]]
            {
                // check if window_ has the specified property
                // set in MainWindow constructor
                if (::GetPropW(previous, appname.data())) [[unlikely]]
                {
                    // show and restore window_
                    ::ShowWindow(previous, SW_RESTORE);
                    // activate, set foreground and get forcus
                    if (::SetForegroundWindow(previous))
                        ::ExitProcess(exitcode);
                }
            }
        }
    }

    HWND GetHandleFromWindow(winrt::Microsoft::UI::Xaml::Window const& window)
    {
        auto handle{ HWND{} };
        window.try_as<IWindowNative>()->get_WindowHandle(&handle);
        return handle;
    }

    double GetScaleAdjustment(winrt::Microsoft::UI::Xaml::Window const& window)
    {
        auto dpi_x{ ::GetDpiForWindow(GetHandleFromWindow(window)) };
        auto scale_factor_percent{ (dpi_x * 100 + (96 >> 1)) / 96 };
        return scale_factor_percent / 100.;
    }

    /// <summary>
    /// Make app only has one instance
    /// call at window_ initialized
    /// </summary>
    /// <param name="appname">size lesser than 16</param>
    /// <param name="window_handle">window_handle of window_</param>
    void DisableMultiInstanceWindow(winrt::Microsoft::UI::Xaml::Window const& window, std::wstring_view const appname)
    {
        auto window_handle{ GetHandleFromWindow(window) };
        ::SetPropW(window_handle, appname.data(), window_handle);
    }

    std::atomic<uintptr_t> old_proc;

    LRESULT CALLBACK WindowProc(HWND window_handle, UINT unsigned_message, WPARAM wide_parameter, LPARAM long_parameter)
    {
        auto scale_factor(::GetDpiForWindow(window_handle));
        if (WM_GETMINMAXINFO == unsigned_message) [[unlikely]]
        {
            reinterpret_cast<MINMAXINFO*>(long_parameter)->ptMinTrackSize.x = (362 * scale_factor * 100 + (96 >> 1)) / 9600;
            reinterpret_cast<MINMAXINFO*>(long_parameter)->ptMinTrackSize.y = (170 * scale_factor * 100 + (96 >> 1)) / 9600;
        }
        return ::CallWindowProcW(reinterpret_cast<WNDPROC>(old_proc.load(std::memory_order_acquire)), window_handle, unsigned_message, wide_parameter, long_parameter);
    }

    void RegisterWindowMinSize(winrt::Microsoft::UI::Xaml::Window const& window)
    {
        old_proc.store(::SetWindowLongPtrW(GetHandleFromWindow(window), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowProc)), std::memory_order_release);
    }

    winrt::hstring GetMusicFolderPath()
    {
        auto path{ PWSTR{} };
#pragma comment(lib, "Shell32.lib")
        if (!::SHGetKnownFolderPath(FOLDERID_Music, 0, NULL, &path)) [[likely]]
        {
            auto result{ winrt::hstring{ path } };
            ::CoTaskMemFree(path);
            return result;
        }
        return {};
    }

    void RegistCoreWindow(winrt::Windows::Foundation::IInspectable const& object)
    {
        object.try_as<IInitializeWithWindow>()->Initialize(GetActiveWindow());
    }

    void OpenExplorer(std::wstring const& path)
    {
        auto info{ SHELLEXECUTEINFOW{} };
        info.cbSize = sizeof(info);
        info.lpFile = L"explorer.exe";
        info.lpParameters = path.c_str();
        info.nShow = SW_SHOWNORMAL;
        ::ShellExecuteExW(&info);
    }
}
