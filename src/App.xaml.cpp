#include "pch.h"

#include "App.xaml.h"
// for ::IWindowNative
#include <Microsoft.UI.Xaml.Window.h>

#include "Win32Helper.h"
#include "SettingsHelper.h"

namespace winrt::Player::implementation
{
    /// <summary>
    /// Initializes the singleton application object. This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    App::App()
    {
        InitializeComponent();

        Win32Helper::DisableMultiInstanceEntry(appname_, 1u);
        // alternate way
        // https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/migrate-to-windows-app-sdk/guides/applifecycle

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::UnhandledExceptionEventArgs const& e) {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
    }

    /// <summary>
    /// Invoked when the application is launched.
    /// Create and activate the window_.
    /// </summary>
    /// <param name="e">
    /// Almost useless.
    /// Unless truely know what LaunchActivatedEventArgs have,
    /// absolutely don't need to use it.
    /// </param>
    void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {

        // lazy initialize window
        window_ = winrt::Microsoft::UI::Xaml::Window{};
        // make root_page
        root_page_ = winrt::Player::RootPage{};
        window_.Content(root_page_);
        auto player_view_model{ root_page_.PlayerViewModel() };
        window_.Title(player_view_model.Title());
        player_view_model.PropertyChanged([&self = *this, ui_thread = winrt::apartment_context{}](winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
            if (args.PropertyName() != L"AppTitle")
                return;
            self.window_.Title(sender.try_as<winrt::Player::PlayerViewModel>().AppTitle());
        });

        auto app_titlebar{ root_page_.AppTitleBar() };

        // initialize AppWindow
        auto app_window{ window_.AppWindow() };
        // https://www.aconvert.com/cn/icon/png-to-ico/
        app_window.SetIcon(L"Assets/PlayerWinRT.ico");

#define Windows10 false
        // test winrt::Windows 10 branch on winrt::Windows 11
        if (winrt::Microsoft::UI::Windowing::AppWindowTitleBar::IsCustomizationSupported() && !Windows10) [[likely]]

#undef Windows10
        {
            auto titlebar{ app_window.TitleBar() };
            titlebar.ExtendsContentIntoTitleBar(true);
            titlebar.PreferredHeightOption(winrt::Microsoft::UI::Windowing::TitleBarHeightOption::Tall);
            app_window.Changed({ this, &App::AppWindow_Changed });
            app_titlebar.Loaded({ this, &App::AppTitleBar_Loaded });
            app_titlebar.SizeChanged({ this, &App::AppTitleBar_SizeChanged });

            // set titlebar theme
            auto theme{ SettingsHelper::LoadTheme() };
            SetTitleBarTheme(titlebar, theme);

            root_page_.ActualThemeChanged([app_window](winrt::Microsoft::UI::Xaml::FrameworkElement const& sender, winrt::Windows::Foundation::IInspectable const&) {
                auto titleBar{ app_window.TitleBar() };
                assert(titleBar.ExtendsContentIntoTitleBar());
                SetTitleBarTheme(titleBar, sender.ActualTheme());
            });
        }
        else
        {
            // In the case that title bar customization is not supported, fallback to WindowChrome
            window_.ExtendsContentIntoTitleBar(true);
            window_.SetTitleBar(root_page_.AppTitleBar());
        }

        // make app only have one instance
        window_.Activated([&self = *this](winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const&) {
            // set window_ property to find instance
            Win32Helper::DisableMultiInstanceWindow(sender.try_as<Microsoft::UI::Xaml::Window>(), self.appname_);
        });

        Win32Helper::RegisterWindowMinSize(window_);

        // active
        window_.Activate();
    }

    void App::AppWindow_Changed(winrt::Microsoft::UI::Windowing::AppWindow sender, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const& args)
    {
        // todo: https://learn.microsoft.com/zh-cn/windows/apps/develop/title-bar?tabs=wasdk#full-customization-example
    }

    void App::AppTitleBar_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        assert(winrt::Microsoft::UI::Windowing::AppWindowTitleBar::IsCustomizationSupported());
        SetDragRegionForCustomTitleBar();
    }

    void App::AppTitleBar_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const&)
    {
        assert(winrt::Microsoft::UI::Windowing::AppWindowTitleBar::IsCustomizationSupported());
        assert(window_.AppWindow().TitleBar().ExtendsContentIntoTitleBar());
        SetDragRegionForCustomTitleBar();
    }

    void App::SetDragRegionForCustomTitleBar()
    {
        assert(winrt::Microsoft::UI::Windowing::AppWindowTitleBar::IsCustomizationSupported());
        auto titleBar{ window_.AppWindow().TitleBar() };
        assert(titleBar.ExtendsContentIntoTitleBar());
        auto scale_adjustment{ Win32Helper::GetScaleAdjustment(window_) };
        auto appTitleBar{ window_.Content().try_as<Player::RootPage>().AppTitleBar() };
        auto rect{ winrt::Windows::Graphics::RectInt32{} };
        rect.X = static_cast<int32_t>((titleBar.LeftInset() + 48) * scale_adjustment);
        rect.Y = 0;
        rect.Height = static_cast<int32_t>(48 * scale_adjustment);
#ifdef _DEBUG
        // make application tool bar clickable
        rect.Width = static_cast<int32_t>(appTitleBar.ActualWidth() * scale_adjustment / 3);
#else
        rect.Width = static_cast<int32_t>(app_titlebar.ActualWidth() * scale_adjustment - rect.X - titlebar.RightInset());
#endif
        titleBar.SetDragRectangles(winrt::array_view(&rect, &rect + 1));
    }

    void App::SetTitleBarTheme(winrt::Microsoft::UI::Windowing::AppWindowTitleBar& titlebar, winrt::Microsoft::UI::Xaml::ElementTheme theme)
    {
        if (theme == winrt::Microsoft::UI::Xaml::ElementTheme::Default) [[likely]]
        {
            titlebar.BackgroundColor(nullptr);
            titlebar.ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
            titlebar.ButtonForegroundColor(nullptr);
            titlebar.ButtonInactiveBackgroundColor(nullptr);
            titlebar.ButtonInactiveForegroundColor(nullptr);
            titlebar.ButtonHoverBackgroundColor(nullptr);
            titlebar.ButtonHoverForegroundColor(nullptr);
            titlebar.ButtonPressedBackgroundColor(nullptr);
            titlebar.ButtonPressedForegroundColor(nullptr);
        }
        else if (theme == winrt::Microsoft::UI::Xaml::ElementTheme::Dark)
        {
            titlebar.BackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
            titlebar.ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
            titlebar.ButtonForegroundColor(winrt::Microsoft::UI::Colors::White());
            titlebar.ButtonInactiveBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
            titlebar.ButtonInactiveForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 96, 96, 96));
            titlebar.ButtonHoverBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 45, 45, 45));
            titlebar.ButtonHoverForegroundColor(winrt::Microsoft::UI::Colors::White());
            titlebar.ButtonPressedBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 41, 41, 41));
            titlebar.ButtonPressedForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 167, 167, 167));
        }
        else if (theme == winrt::Microsoft::UI::Xaml::ElementTheme::Light)
        {
            titlebar.BackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
            titlebar.ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
            titlebar.ButtonForegroundColor(winrt::Microsoft::UI::Colors::Black());
            titlebar.ButtonInactiveBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 243, 243, 243));
            titlebar.ButtonInactiveForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 144, 144, 144));
            titlebar.ButtonHoverBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 219, 219, 219));
            titlebar.ButtonHoverForegroundColor(winrt::Microsoft::UI::Colors::Black());
            titlebar.ButtonPressedBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 194, 194, 194));
            titlebar.ButtonPressedForegroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(255, 95, 95, 95));
        }
    }
}
