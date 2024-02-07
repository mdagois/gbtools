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
	TileFlip& out_tile_flip, uint32_t& out_palette_index,
	const ImageTile& image_tile, const PaletteSet& palette_set)
{
	{
		Palette tile_palette;
		if(kSuccess != extractTilePalette(tile_palette, image_tile))
		{
			return false;
		}

		const uint32_t palette_index = palette_set.findCompatiblePaletteIndex(tile_palette);
		if(palette_index == kInvalidPaletteIndex)
		{
			return false;
		}
		out_palette_index = palette_index;
	}

	const Palette& palette = palette_set[out_palette_index];
	for(uint32_t i = 0; i < kPixelsPerTile; ++i)
	{
		const uint8_t color_index = palette.findColorIndex(image_tile[i]);
		if(color_index == kInvalidColorIndex)
		{
			return false;
		}
		out_tile_flip.color_indices[i] = color_index;
	}
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

