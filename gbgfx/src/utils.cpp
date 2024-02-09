#include <set>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb_image_write.h"

#include "constants.h"
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

static Result generateTileFlip(
	TileFlip& out_tile_flip, uint32_t& out_palette_index,
	const ImageTile& image_tile, const PaletteSet& palette_set)
{
	{
		Palette tile_palette;
		const Result result = extractTilePalette(tile_palette, image_tile);
		if(result != kSuccess)
		{
			return result;
		}

		const uint32_t palette_index = palette_set.findCompatiblePaletteIndex(tile_palette);
		if(palette_index == kInvalidPaletteIndex)
		{
			return kError_CouldNotFindCompatiblePalette;
		}
		out_palette_index = palette_index;
	}

	const Palette& palette = palette_set[out_palette_index];
	for(uint32_t i = 0; i < kPixelsPerTile; ++i)
	{
		const uint8_t color_index = palette.findColorIndex(image_tile[i]);
		if(color_index == kInvalidColorIndex)
		{
			return kError_CouldNotFindColorInPalette;
		}
		out_tile_flip.color_indices[i] = color_index;
	}
	return kSuccess;
}

Result generateTile(Tile& out_tile, const ImageTile& image_tile, const PaletteSet& palette_set)
{
	TileFlip tile_flip;
	uint32_t palette_index;
	const Result result = generateTileFlip(tile_flip, palette_index, image_tile, palette_set);
	if(result != kSuccess)
	{
		return result;
	}
	out_tile.initialize(tile_flip, palette_index);
	return kSuccess;
}

////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kChannelCount = 4,
};

static void blitTile(ColorRGBA* out_pixels, uint32_t pitch, const TileFlip& flip, const Palette& palette)
{
	const uint8_t* indices = flip.color_indices;
	for(uint32_t j = 0; j < kTileSize; ++j)
	{
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			assert(indices[i] < kColorsPerPalette);
			out_pixels[i] = palette[indices[i]];
		}

		indices += kTileSize;
		out_pixels += pitch;
	}
}

static bool findTileFlipInTileset(const Tileset& tileset, uint32_t tile_count, const TileFlip& tile_flip, uint32_t palette_index)
{
	assert(tile_count <= tileset.size());
	for(uint32_t i = 0; i < tile_count; ++i)
	{
		const Tile& tile = tileset[i];
		if(palette_index == tile.getPaletteIndex() && tile_flip == tile.getTileFlip(kTileFlipType_None))
		{
			return true;
		}
	}
	return false;
}

Result writeTilesetToPNG(
	const char* filename, uint32_t tile_column_count,
	const Tileset& tileset, TileFlipType flip_type, const PaletteSet& palette_set,
	bool clear_doubles)
{
	assert(flip_type < kTileFlipType_Count);

	const uint32_t tile_row_count = tileset.size() / tile_column_count + (tileset.size() % tile_column_count == 0 ? 0 : 1);
	const int32_t image_width = static_cast<int32_t>(tile_column_count * kTileSize);
	const int32_t image_height = static_cast<int32_t>(tile_row_count * kTileSize);

	TileFlip empty_flip;
	for(uint32_t i = 0; i < kPixelsPerTile; ++i)
	{
		empty_flip.color_indices[i] = 0;
	}
	Palette empty_palette;
	empty_palette.push(kRGBA_Magenta);

	ColorRGBA* pixels = new ColorRGBA[image_width * image_height];

	for(uint32_t i = 0; i < tileset.size(); ++i)
	{
		const Tile& tile = tileset[i];
		const uint32_t palette_index = tile.getPaletteIndex();
		const Palette& palette = palette_set[palette_index];
		const TileFlip& flip = tile.getTileFlip(flip_type);

		const Palette* blit_palette = &palette;
		const TileFlip* blit_flip = &flip;
		if(clear_doubles && findTileFlipInTileset(tileset, i, flip, palette_index))
		{
			blit_palette = &empty_palette;
			blit_flip = &empty_flip;
		}
		const uint32_t tile_row = i / tile_column_count;
		const uint32_t tile_column = i % tile_column_count;
		blitTile(pixels + (tile_row * kTileSize * image_width) + (tile_column * kTileSize), image_width, *blit_flip, *blit_palette);
	}

	const int result = stbi_write_png(
		filename,
		image_width, image_height, kChannelCount,
		pixels, sizeof(ColorRGBA) * image_width);

	delete [] pixels;
	return result != 0 ? kSuccess : kError_CouldNotWriteTilesetToFile;
}

