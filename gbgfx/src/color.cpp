#include <cassert>
#include "color.h"

////////////////////////////////////////////////////////////////////////////////

double getLuminance(const ColorBGR555 color)
{
	const double blue = ((color >> 10) & 0x1F) / 32.0;
	const double green = ((color >> 5) & 0x1F) / 32.0;
	const double red = (color & 0x1F) / 32.0;
	return 0.2126 * red + 0.7152 * green + 0.0722 * blue;
}

double getLuminance(const ColorRGBA color)
{
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

ColorBGR555 convertColor(ColorRGBA rgba)
{
	const uint8_t red = rgba.r / 8;
	const uint8_t green = rgba.g / 8;
	const uint8_t blue = rgba.b / 8;
	assert(red < 32 && green < 32 && blue < 32);
	return (blue << 10) | (green << 5) | red;
}

