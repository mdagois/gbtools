#include <set>
#include "utils.h"

Result extractTilePalette(Palette& out_tile_palette, const ImageTile& tile)
{
	std::set<ColorRGBA> colors;
	for(uint32_t i = 0; i < kPixelsPerTile; ++i)
	{
		colors.insert(tile[i]);
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

bool generateTile(Tile& out_tile, const ImageTile& image_tile, const PaletteSet& palette_set)
{
	//TODO
	Tile tile = {};
	out_tile = tile;
	return true;
}

