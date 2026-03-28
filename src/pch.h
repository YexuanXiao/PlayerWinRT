#pragma once

#define NOMINMAX

#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Storage.AccessCache.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.PlayBack.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.Search.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Core.h>

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

#include <pplawait.h>

#include <regex>
#include <format>
#include <ranges>

#include <wil/cppwinrt_helpers.h>


inline winrt::hstring operator""_hs(wchar_t const* begin, std::size_t const size)
{
    return winrt::hstring{ begin, static_cast<winrt::hstring::size_type>(size) };
}

namespace util
{
    template <class... Args> inline winrt::hstring hformat(std::wstring_view fmt, Args&&... args)
    {
        return winrt::hstring{ std::vformat(fmt, std::make_wformat_args(args...)) };
    }
    struct fire_and_forget {
        struct promise_type {
            fire_and_forget get_return_object() const noexcept { return {}; }
            std::suspend_never initial_suspend() const noexcept { return {}; }
            std::suspend_never final_suspend() const noexcept { return {}; }
            void return_void() const noexcept {}
            void unhandled_exception() const noexcept {}
        };
    };
}