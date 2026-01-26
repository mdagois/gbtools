#include <algorithm>
#include <cassert>

#include "color.h"

namespace gfx {

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
			if(lhs == kRGBA_Magenta)
			{
				return true;
			}
			if(rhs == kRGBA_Magenta)
			{
				return false;
			}
			return getLuminance(lhs) < getLuminance(rhs);
		});
}

////////////////////////////////////////////////////////////////////////////////

}

