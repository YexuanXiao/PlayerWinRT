#pragma once
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

#include <fast_io.h>

namespace winrt::Player {}

inline winrt::hstring operator""_hs(const wchar_t* begin, std::size_t size) {
	return winrt::hstring{ begin, static_cast<winrt::hstring::size_type>(size) };
}

#ifdef _MSC_VER
#if _MSC_VER <= 1937

// for C++ 23 std::size_t literal workaround

inline constexpr std::size_t operator""uz(unsigned long long value) {
	if constexpr (sizeof(unsigned long long) <= sizeof(std::size_t)) {
		return std::size_t{ value };
	}
	else {
		assert(value < SIZE_MAX);
		return std::size_t{ value };
	}
}

#endif
#endif
