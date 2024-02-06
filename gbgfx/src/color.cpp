#include <algorithm>
#include <cassert>
#include "color.h"

////////////////////////////////////////////////////////////////////////////////

static bool operator==(const ColorRGBA& lhs, const ColorRGBA& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

static ColorRGBA kRGBA_Magenta = {0xFFU, 0x00U, 0xFFU, 0xFFU};

double getLuminance(const ColorRGBA color)
{
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void sortColors(ColorRGBA* colors, uint32_t count)
{
	std::sort(
		colors, colors + count,
		[](const ColorRGBA lhs, const ColorRGBA rhs)
		{
			if(lhs == kRGBA_Magenta || rhs == kRGBA_Invalid)
			{
				return true;
			}
			if(lhs == kRGBA_Invalid || rhs == kRGBA_Magenta)
			{
				return false;
			}
			return getLuminance(lhs) < getLuminance(rhs);
		});
}

////////////////////////////////////////////////////////////////////////////////

enum : uint16_t
{
	kBGR555_Magenta = 0x7C1FU,
};

double getLuminance(const ColorBGR555 color)
{
	const double blue = ((color >> 10) & 0x1F) / 32.0;
	const double green = ((color >> 5) & 0x1F) / 32.0;
	const double red = (color & 0x1F) / 32.0;
	return 0.2126 * red + 0.7152 * green + 0.0722 * blue;
}

void sortColors(ColorBGR555* colors, uint32_t count)
{
	std::sort(
		colors, colors + count,
		[](const ColorBGR555 lhs, const ColorBGR555 rhs)
		{
			if(lhs == kBGR555_Magenta)
			{
				return true;
			}
			if(rhs == kBGR555_Magenta)
			{
				return false;
			}
			return getLuminance(lhs) < getLuminance(rhs);
		});
}

////////////////////////////////////////////////////////////////////////////////

ColorBGR555 convertColor(ColorRGBA rgba)
{
	const uint8_t red = rgba.r / 8;
	const uint8_t green = rgba.g / 8;
	const uint8_t blue = rgba.b / 8;
	assert(red < 32 && green < 32 && blue < 32);
	return (blue << 10) | (green << 5) | red;
}

