#include <set>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb_image_write.h"

#include "constants.h"
#include "log.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////

bool extractTilePalette(Palette& out_tile_palette, const ImageTile& tile)
{
	std::set<ColorRGBA> colors;
	for(uint32_t i = 0; i < kPixelsPerTile; ++i)
	{
		colors.insert(tile[i]);
	}
	if(colors.size() > kColorsPerPalette)
	{
		LOG_ERROR("Too many colors in palette");
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
			LOG_ERROR("Could not extract palette");
			return false;
		}

		const uint32_t palette_index = palette_set.findCompatiblePaletteIndex(tile_palette);
		if(palette_index == kInvalidPaletteIndex)
		{
			LOG_ERROR("Could not find a compatible palette");
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
			LOG_ERROR("Could not find color in palette");
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
		LOG_ERROR("Could not generate tile flip");
		return false;
	}
	out_tile.initialize(tile_flip, palette_index);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

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

static bool findTileFlipInTileset(
	const Tileset& tileset, uint32_t tile_count,
	const TileFlip& tile_flip, TileFlipType flip_type,
	uint32_t palette_index)
{
	assert(tile_count <= tileset.size());
	for(uint32_t i = 0; i < tile_count; ++i)
	{
		const Tile& tile = tileset[i];
		if(palette_index == tile.getPaletteIndex() && tile_flip == tile.getTileFlip(flip_type))
		{
			return true;
		}
	}
	return false;
}

bool writeTilesetToPNG(
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

	const uint32_t tile_remainder = tileset.size() % tile_column_count;
	const uint32_t image_tile_count = tileset.size() + (tile_remainder == 0 ? 0 : (tile_column_count - tile_remainder));
	assert(image_tile_count % tile_column_count == 0);
	for(uint32_t i = 0; i < image_tile_count; ++i)
	{
		const Palette* blit_palette = &empty_palette;
		const TileFlip* blit_flip = &empty_flip;
		if(i < tileset.size())
		{
			const Tile& tile = tileset[i];
			const uint32_t palette_index = tile.getPaletteIndex();
			const Palette& palette = palette_set[palette_index];
			const TileFlip& flip = tile.getTileFlip(flip_type);
			if(!clear_doubles || !findTileFlipInTileset(tileset, i, flip, flip_type, palette_index))
			{
				blit_palette = &palette;
				blit_flip = &flip;
			}
		}
		assert(blit_palette != nullptr);
		assert(blit_flip != nullptr);

		const uint32_t tile_row = i / tile_column_count;
		const uint32_t tile_column = i % tile_column_count;
		blitTile(
			pixels + (tile_row * kTileSize * image_width) + (tile_column * kTileSize),
			image_width, *blit_flip, *blit_palette);
	}

	constexpr int32_t kChannelCount = 4;
	const int result = stbi_write_png(
		filename,
		image_width, image_height, kChannelCount,
		pixels, sizeof(ColorRGBA) * image_width);

	delete [] pixels;
	if(result == 0)
	{
		LOG_ERROR("Could not write file [" << filename << "]");
		return false;
	}
	return true;
}

