#include <set>
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

static bool generateTileFlip(
	TileFlip& out_tile_flip, uint32_t& palette_index,
	const ImageTile& image_tile, const PaletteSet& palette_set)
{
	//TODO Generate the tile flip
	return true;
}

bool generateTile(Tile& out_tile, const ImageTile& image_tile, const PaletteSet& palette_set)
{
	TileFlip tile_flip;
	uint32_t palette_index;
	if(!generateTileFlip(tile_flip, palette_index, image_tile, palette_set))
	{
		return false;
	}

	out_tile.initialize(tile_flip, palette_index);
	return true;
}

