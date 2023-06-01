#include "pch.h"
#include "InfoListItem.xaml.h"
#if __has_include("InfoListItem.g.cpp")
#include "InfoListItem.g.cpp"
#endif

namespace winrt::Player::implementation
{
    InfoListItem::InfoListItem()
    {
        InitializeComponent();
    }

    winrt::hstring InfoListItem::Track()
    {

        return TrackUI().Text();
    }

    winrt::hstring InfoListItem::Title()
    {
        return TitleUI().Text();
    }

    winrt::hstring InfoListItem::Artist()
    {

        return ArtistUI().Text();
    }

    winrt::hstring InfoListItem::State()
    {
        return StateUI().Glyph();
    }

    winrt::hstring InfoListItem::Bitrate()
    {
        return BitrateUI().Text();
    }

    winrt::hstring InfoListItem::Duration()
    {
        return DurationUI().Text();
    }

    void InfoListItem::Track(winrt::hstring const& value)
    {
        TrackUI().Text(value);
    }

    void InfoListItem::Title(winrt::hstring const& value)
    {

        TitleUI().Text(value);
    }

    void InfoListItem::Artist(winrt::hstring const& value)
    {
        ArtistUI().Text(value);

        auto title_ui{ TitleUI() };
        if (value.empty())
            winrt::Microsoft::UI::Xaml::Controls::Grid::SetRowSpan(title_ui, 2);
        else
            winrt::Microsoft::UI::Xaml::Controls::Grid::SetRowSpan(title_ui, 1);
    }

    void InfoListItem::State(winrt::hstring const& value)
    {
        StateUI().Glyph(value);
    }

    void InfoListItem::Bitrate(winrt::hstring const& value)
    {
        BitrateUI().Text(value);
    }

    void InfoListItem::Duration(winrt::hstring const& value)
    {
        DurationUI().Text(value);
    }
}
