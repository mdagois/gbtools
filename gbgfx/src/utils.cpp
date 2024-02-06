#include <set>
#include "utils.h"

static bool operator<(const ColorRGBA& lhs, const ColorRGBA& rhs)
{
    return getLuminance(lhs) < getLuminance(rhs);
}

Result extractTilePalette(Palette& out_tile_palette, const ImageTile& tile)
{
	std::set<ColorRGBA> colors;
	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			colors.insert(tile[j][i]);
		}
	}
	if(colors.size() > kColorsPerPalette)
	{
		return kError_TooManyColorInPalette;
	}

	for(ColorRGBA color : colors)
	{
		out_tile_palette.push(color);
	}
	return kSuccess;
}

