#include <cassert>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb_image_write.h"

#include "api.h"
#include "export.h"
#include "image.h"
#include "log.h"
#include "utils.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////////////////////


static bool areSingleColorTiles(const std::vector<ImageTile>& tiles)
{
	assert(tiles.size() > 0);
	const ColorRGBA& single_color = tiles[0][0];
	for(size_t t = 0; t < tiles.size(); ++t)
	{
		const ImageTile& tile = tiles[t];
		for(uint32_t c = 0; c < tile.size(); ++c)
		{
			if(tile[c] != single_color)
			{
				return false;
			}
		}
	}
	return true;
}

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	bool skip_single_color_metatiles, bool use_microtile_8x16,
	bool remove_doubles, bool remove_flips,
	const char* image_filename)
{
	Image image;
	if(!image.read(image_filename))
	{
		return false;
	}

	const uint32_t tiles_per_metatile = (metatile_width / kTileSize) * (metatile_height / kTileSize);
	std::vector<ImageTile> metatile_tiles;
	std::vector<ImageTile> image_tiles;
	if(!image.iterateTiles(
		start_tile_row, tile_row_count,
		metatile_width, metatile_height,
		use_microtile_8x16,
		[&metatile_tiles, &image_tiles, &out_palette_set,
		 image_filename, tiles_per_metatile, skip_single_color_metatiles](const ImageTile& tile, uint32_t x, uint32_t y)
		{
			Palette palette;
			if(!extractTilePalette(palette, tile))
			{
				GBGFX_LOG_ERROR(
					"Could not extract palette from metatile ("
					<< x << "," << y << ") in [" << image_filename << "]");
				return false;
			}
			out_palette_set.push(palette);
			metatile_tiles.push_back(tile);
			if(metatile_tiles.size() == tiles_per_metatile)
			{
				if(!skip_single_color_metatiles || !areSingleColorTiles(metatile_tiles))
				{
					for(auto& metatile_tile : metatile_tiles)
					{
						image_tiles.push_back(metatile_tile);
					}
				}
				metatile_tiles.clear();
			}
			return true;
		}))
	{
		return false;
	}

	out_palette_set.optimize(true, true);

	for(uint32_t i = 0; i < image_tiles.size(); ++i)
	{
		Tile tile;
		if(!generateTile(tile, image_tiles[i], out_palette_set))
		{
			GBGFX_LOG_ERROR(
				"Could not generate tile ("
				<< i << ") in [" << image_filename << "]");
			return false;
		}
		out_tileset.push(tile);
	}

	if(remove_doubles)
	{
		out_tileset.removeDoubles(remove_flips);
	}

	GBGFX_LOG_INFO(
		"Tile count is " << out_tileset.size()
		<< " and palette count is " << out_palette_set.size()
		<< " in [" << image_filename << "]"); 
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool extractTilemap(
	Tilemap& out_tilemap,
	const Tileset& tileset, const PaletteSet& palette_set,
	bool use_flips,
	const char* image_filename)
{
	Image image;
	if(!image.read(image_filename))
	{
		return false;
	}

	if(!out_tilemap.initialize(image.getHeight() / kTileSize, image.getWidth() / kTileSize))
	{
		GBGFX_LOG_ERROR("Could not initialize tilemap from [" << image_filename << "]");
		return false;
	}
	if(!image.iterateTiles(
		[&out_tilemap, &tileset, &palette_set, use_flips, image_filename](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Tile tile;
			if(!generateTile(tile, image_tile, palette_set))
			{
				GBGFX_LOG_ERROR(
					"Could not generate tile ("
					<< x << "," << y << ") in ["
					<< image_filename << "]");
				return false;
			}

			uint32_t tile_index;
			uint32_t palette_index;
			TileFlipType flip_type;
			if(!tileset.findTileIndex(tile_index, palette_index, flip_type, tile, use_flips))
			{
				GBGFX_LOG_ERROR(
					"Could not find tile ("
					<< x << "," << y << ") in tileset in ["
					<< image_filename << "]");
				return false;
			}

			assert(tile_index <= kTileMaxCount);
			constexpr uint32_t priority = 0;
			const uint32_t bank = tile_index < kTilesPerBank ? 0 : 1;
			out_tilemap.push(
				tile_index, palette_index, bank,
				flip_type == kTileFlipType_Horizontal || flip_type == kTileFlipType_Both,
				flip_type == kTileFlipType_Vertical || flip_type == kTileFlipType_Both,
				priority);
			return true;
		}))
	{
		return false;
	}

	GBGFX_LOG_INFO(
		"Tilemap size is " << out_tilemap.getColumnCount() << "x" << out_tilemap.getRowCount()
		<< " in [" << image_filename << "]"); 
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Output
////////////////////////////////////////////////////////////////////////////////

static bool writeToFile(
	const void* data, uint32_t size,
	const void* header, uint32_t header_size,
	const char* output_filename)
{
	assert(output_filename != nullptr);

	FILE* output_file = fopen(output_filename, "wb");
	if(output_file == nullptr)
	{
		GBGFX_LOG_ERROR("Could not open file [" << output_filename << "]");
		return false;
	}
	assert(output_file != nullptr);

	if(size == 0)
	{
		fclose(output_file);
	}

	const bool written =
		(header == nullptr || fwrite(header, header_size, 1, output_file) == 1) &&
		fwrite(data, size, 1, output_file) == 1;
	fclose(output_file);
	if(!written)
	{
		GBGFX_LOG_ERROR("Could not write file [" << output_filename << "]");
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool exportPaletteSet(
	const PaletteSet& palette_set,
	const char* output_filename, bool use_header)
{
	PaletteSetData data;
	assert(palette_set.size() < 256);
	const uint8_t palette_count = palette_set.size();
	return
		data.initialize(palette_set) &&
		writeToFile(
			data.getData(), data.getDataSize(),
			use_header ? &palette_count : nullptr, sizeof(palette_count),
			output_filename);
}

////////////////////////////////////////////////////////////////////////////////

bool exportTileset(
	const Tileset& tileset,
	const char* output_filename, bool use_header)
{
	TilesetData data;
	assert(tileset.size() < 65536);
	const uint16_t tile_count = static_cast<uint16_t>(tileset.size());
	return
		data.initialize(tileset) &&
		writeToFile(
			data.getData(), data.getDataSize(),
			use_header ? &tile_count : nullptr, sizeof(tile_count),
			output_filename);
}

////////////////////////////////////////////////////////////////////////////////

bool exportTilemap(
	const Tilemap& tilemap,
	const char* indices_filename, const char* parameter_filename,
	bool use_header, uint8_t palette_index_offset, uint8_t tile_index_offset)
{
	TilemapData data;
	assert(tilemap.getColumnCount() < 256);
	assert(tilemap.getRowCount() < 256);
	const uint8_t header[2] =
	{
		static_cast<uint8_t>(tilemap.getColumnCount()),
		static_cast<uint8_t>(tilemap.getRowCount())
	};
	return
		data.initialize(tilemap, palette_index_offset, tile_index_offset) &&
		(
			indices_filename == nullptr ||
			writeToFile(
				data.getIndexData(), data.getDataSize(),
				use_header ? &header : nullptr, sizeof(header),
				indices_filename))
		&&
		(
			parameter_filename == nullptr ||
			writeToFile(
				data.getParameterData(), data.getDataSize(),
				use_header ? &header : nullptr, sizeof(header),
				parameter_filename
		));
}

////////////////////////////////////////////////////////////////////////////////
// Debug
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
		GBGFX_LOG_ERROR("Could not write file [" << filename << "]");
		return false;
	}
	return true;
}

bool writePaletteSetToPNG(const char* filename, const PaletteSet& palette_set)
{
	const uint32_t palette_count = palette_set.size();
	if(palette_count == 0)
	{
		return true;
	}

	const uint32_t total_color_count = kColorsPerPalette * palette_count;
	ColorRGBA* pixels = new ColorRGBA[total_color_count];
	for(uint32_t p = 0; p < palette_count; ++p)
	{
		const Palette& palette = palette_set[p];
		uint32_t c = 0;
		for(; c < palette.size(); ++c)
		{
			pixels[p * kColorsPerPalette + c] = palette[c];
		}
		for(; c < kColorsPerPalette; ++c)
		{
			pixels[p * kColorsPerPalette + c] = kRGBA_Magenta;
		}
	}

	constexpr int32_t kChannelCount = 4;
	const int result = stbi_write_png(
		filename,
		kColorsPerPalette, palette_count, kChannelCount,
		pixels, sizeof(ColorRGBA) * total_color_count);

	delete [] pixels;
	if(result == 0)
	{
		GBGFX_LOG_ERROR("Could not write file [" << filename << "]");
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

}
