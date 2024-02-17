#include <set>

#include "api.h"
#include "constants.h"
#include "log.h"
#include "utils.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

bool extractTilePalette(Palette& out_tile_palette, const ImageTile& tile)
{
	std::set<ColorRGBA> colors;
	for(uint32_t i = 0; i < kPixelsPerTile; ++i)
	{
		colors.insert(tile[i]);
	}
	if(colors.size() > getPaletteColorMaxCount())
	{
		GBGFX_LOG_ERROR("Too many colors in palette");
		return false;
	}

	for(ColorRGBA color : colors)
	{
		out_tile_palette.push(color);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

static bool generateTileFlip(
	TileFlip& out_tile_flip, uint32_t& out_palette_index,
	const ImageTile& image_tile, const PaletteSet& palette_set)
{
	{
		Palette tile_palette;
		if(!extractTilePalette(tile_palette, image_tile))
		{
			GBGFX_LOG_ERROR("Could not extract palette");
			return false;
		}

		const uint32_t palette_index = palette_set.findCompatiblePaletteIndex(tile_palette);
		if(palette_index == kInvalidPaletteIndex)
		{
			GBGFX_LOG_ERROR("Could not find a compatible palette");
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
			GBGFX_LOG_ERROR("Could not find color in palette");
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
		GBGFX_LOG_ERROR("Could not generate tile flip");
		return false;
	}
	out_tile.initialize(tile_flip, palette_index);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

}

