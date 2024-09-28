#include <cassert>

#include "color.h"

namespace gfx {

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
// Color index from a fixed color palette.
////////////////////////////////////////////////////////////////////////////////

static uint64_t computeDistance(ColorRGBA lhs, ColorRGBA rhs)
{
	const int32_t lr = lhs.r;
	const int32_t lg = lhs.g;
	const int32_t lb = lhs.b;

	const int32_t rr = lhs.r;
	const int32_t rg = lhs.g;
	const int32_t rb = lhs.b;

	const int32_t dr = rr - lr;
	const int32_t dg = rg - lg;
	const int32_t db = rb - lb;

	return dr * dr + dg * dg + db * db;
}

ColorIndex convertColor(ColorRGBA rgba, const ColorRGBA* colors, uint32_t color_count)
{
	assert(colors != nullptr);
	assert(color_count > 0);

	ColorIndex index = 0;
	uint64_t shortest_distance = computeDistance(rgba, colors[0]);
	for(uint32_t i = 1; i < color_count; ++i)
	{
		const uint64_t distance = computeDistance(rgba, colors[i]);
		if(distance < shortest_distance)
		{
			shortest_distance = distance;
			index = i;
		}
	}
	return index;
}

////////////////////////////////////////////////////////////////////////////////

}

