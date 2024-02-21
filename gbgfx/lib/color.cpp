#include <algorithm>
#include <cassert>

#include "color.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// RGBA
////////////////////////////////////////////////////////////////////////////////

bool operator==(const ColorRGBA& lhs, const ColorRGBA& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

bool operator!=(const ColorRGBA& lhs, const ColorRGBA& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const ColorRGBA& lhs, const ColorRGBA& rhs)
{
    return getLuminance(lhs) < getLuminance(rhs);
}

double getLuminance(const ColorRGBA color)
{
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void sortColorsRGBA(ColorRGBA* colors, uint32_t count)
{
	std::sort(
		colors, colors + count,
		[](const ColorRGBA lhs, const ColorRGBA rhs)
		{
			if(lhs.a == kHighPriorityColorAlpha)
			{
				return true;
			}
			if(rhs.a == kHighPriorityColorAlpha)
			{
				return false;
			}
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
// BGR555
////////////////////////////////////////////////////////////////////////////////

ColorBGR555 convertColor(ColorRGBA rgba)
{
	const uint8_t red = rgba.r / 8;
	const uint8_t green = rgba.g / 8;
	const uint8_t blue = rgba.b / 8;
	assert(red < 32 && green < 32 && blue < 32);
	return (blue << 10) | (green << 5) | red;
}

////////////////////////////////////////////////////////////////////////////////

}

