#include <cassert>
#include <set>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb_image_write.h"

#include "export/export.h"
#include "features.h"
#include "gbgfx.h"
#include "import/image.h"
#include "utils/log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////

bool initialize(Hardware hardware)
{
	assert(!areFeaturesInitialized());
	return initializeFeatures(hardware);
}

////////////////////////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////////////////////////

static bool extractTilePalette(Palette& out_tile_palette, const ImageTile& image_tile)
{
	std::set<ColorRGBA> colors;
	const uint32_t pixel_count = image_tile.getWidth() * image_tile.getHeight();
	for(uint32_t i = 0; i < pixel_count; ++i)
	{
		colors.insert(image_tile[i]);
	}
	if(colors.size() > FEATURES.palette.color_max_count)
	{
		GBGFX_LOG_ERROR("Too many colors in image tile");
		return false;
	}
	for(ColorRGBA color : colors)
	{
		out_tile_palette.add(color);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

static bool generateTileFlip(
	TileFlip& out_tile_flip, uint32_t& out_palette_index,
	const ImageTile& image_tile, const PaletteSet& palette_set)
{
	{
		Palette tile_palette(FEATURES.palette.insert_transparent_color);
		if(!extractTilePalette(tile_palette, image_tile))
		{
			GBGFX_LOG_ERROR("Could not extract palette");
			return false;
		}

		uint32_t palette_index = 0;
		if(!palette_set.findCompatiblePaletteIndex(palette_index, tile_palette))
		{
			GBGFX_LOG_ERROR("Could not find a compatible palette");
			return false;
		}
		out_palette_index = palette_index;
	}

	initializeTileFlip(out_tile_flip, image_tile.getWidth(), image_tile.getHeight());

	const Palette& palette = palette_set[out_palette_index];
	const uint32_t pixel_count = image_tile.getWidth() * image_tile.getHeight();
	for(uint32_t i = 0; i < pixel_count; ++i)
	{
		uint8_t color_index = 0;
		if(!palette.findColorIndex(color_index, image_tile[i]))
		{
			GBGFX_LOG_ERROR("Could not find color in palette");
			return false;
		}
		out_tile_flip.color_indices[i] = color_index;
	}
	return true;
}

static bool generateTile(Tile& out_tile, const ImageTile& image_tile, const PaletteSet& palette_set)
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

static bool addBasicTileSize(std::vector<Division>& divisions)
{
	const Division& last = divisions.back();
	if(last.width == FEATURES.tileset.basic_tile_width && last.height == FEATURES.tileset.basic_tile_height)
	{
		return true;
	}
	if(last.width < FEATURES.tileset.basic_tile_width || last.height < FEATURES.tileset.basic_tile_height)
	{
		GBGFX_LOG_ERROR(
			"The last division " << last.width << "x" << last.height
			<< " is smaller than the basic tile "
			<< FEATURES.tileset.basic_tile_width << "x" << FEATURES.tileset.basic_tile_height);
		return false;
	}
	divisions.push_back({ FEATURES.tileset.basic_tile_width, FEATURES.tileset.basic_tile_height, false });
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Import
////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set, ImageInfo& out_image_info,
	const std::vector<Division>& divisions,
	TileRemoval tile_removal, const char* image_filename)
{
	assert(areFeaturesInitialized());
	Image image;
	if(!image.read(image_filename))
	{
		return false;
	}
	return extractTileset(
		out_tileset, out_palette_set, out_image_info,
		divisions, tile_removal, image);
}

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set, ImageInfo& out_image_info,
	const std::vector<Division>& divisions,
	TileRemoval tile_removal, const Image& image)
{
	assert(areFeaturesInitialized());

	std::vector<Division> final_divisions = divisions;
	if(!addBasicTileSize(final_divisions))
	{
		return false;
	}

	if(!image.iterateTiles(
		nullptr,
		final_divisions.data(), static_cast<uint32_t>(final_divisions.size()),
		[&out_palette_set, &image](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Palette palette(FEATURES.palette.insert_transparent_color);
			if(!extractTilePalette(palette, image_tile))
			{
				GBGFX_LOG_ERROR(
					"Could not extract palette from tile ("
					<< x << "," << y << ") in [" << image.getFilename() << "]");
				return false;
			}
			out_palette_set.add(palette);
			return true;
		}))
	{
		return false;
	}

	if(!out_palette_set.optimize(FEATURES.palette.color_max_count, FEATURES.palette.share_first_color, true))
	{
		return false;
	}

	if(!image.iterateTiles(
		&out_image_info,
		final_divisions.data(), static_cast<uint32_t>(final_divisions.size()),
		[&out_tileset, &out_palette_set, &image](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Tile tile;
			if(!generateTile(tile, image_tile, out_palette_set))
			{
				GBGFX_LOG_ERROR(
					"Could not generate tile ("
					<< x << "," << y << ") in [" << image.getFilename() << "]");
				return false;
			}
			out_tileset.add(tile);
			return true;
		}))
	{
		return false;
	}

	if(tile_removal != kTileRemovalNone)
	{
		if(FEATURES.tileset.supports_tile_removal)
		{
			const bool has_flips =
				(FEATURES.tilemap.enabled && FEATURES.tilemap.supports_tile_flips) ||
				(FEATURES.sprite.enabled && FEATURES.sprite.supports_tile_flips);
			if(!has_flips && tile_removal == kTileRemovalFlips)
			{
				GBGFX_LOG_WARN(
					"Downgraded tile removal from " << tile_removal
					<< " to " << kTileRemovalDoubles
					<< " as the hardware does not support flips");
				tile_removal = kTileRemovalDoubles;
			}
			out_tileset.removeDoubles(tile_removal == kTileRemovalFlips);
		}
		else
		{
			GBGFX_LOG_WARN(
				"Ignored tile removal mode " << tile_removal
				<< " as the hardware does not support tile removal");
		}
	}

	GBGFX_LOG_INFO(
		"Tile count is " << out_tileset.size()
		<< " and palette count is " << out_palette_set.size()
		<< " in [" << image.getFilename() << "]"); 
	if(out_tileset.size() > FEATURES.tileset.tile_max_count)
	{
		GBGFX_LOG_INFO(
			"The tile count " << out_tileset.size()
			<< " is over the maximum of " << FEATURES.tileset.tile_max_count);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool extractTilemap(
	Tilemap& out_tilemap, ImageInfo& out_image_info,
	const Tileset& tileset, const PaletteSet& palette_set,
	const std::vector<Division>& divisions,
	const char* image_filename)
{
	assert(areFeaturesInitialized());
	Image image;
	if(!image.read(image_filename))
	{
		return false;
	}
	return extractTilemap(out_tilemap, out_image_info, tileset, palette_set, divisions, image);
}

bool extractTilemap(
	Tilemap& out_tilemap, ImageInfo& out_image_info,
	const Tileset& tileset, const PaletteSet& palette_set,
	const std::vector<Division>& divisions,
	const Image& image)
{
	assert(areFeaturesInitialized());

	std::vector<Division> final_divisions = divisions;
	if(!addBasicTileSize(final_divisions))
	{
		return false;
	}

	if(!image.iterateTiles(
		&out_image_info,
		final_divisions.data(), static_cast<uint32_t>(final_divisions.size()),
		[&out_tilemap, &tileset, &palette_set, &image](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Tile tile;
			if(!generateTile(tile, image_tile, palette_set))
			{
				GBGFX_LOG_ERROR(
					"Could not generate tile ("
					<< x << "," << y << ") in [" << image.getFilename() << "]");
				return false;
			}

			uint32_t tile_index;
			uint32_t palette_index;
			TileFlipType flip_type;
			if(!tileset.findTileIndex(
				tile_index, palette_index, flip_type, tile,
				FEATURES.tilemap.supports_tile_flips))
			{
				GBGFX_LOG_ERROR(
					"Could not find tile ("
					<< x << "," << y << ") in tileset from [" << image.getFilename() << "]");
				return false;
			}

			if(tile_index >= FEATURES.tileset.tile_max_count)
			{
				GBGFX_LOG_ERROR(
					"The tile index [" << tile_index
					<< "] is over the tile max count [" << FEATURES.tileset.tile_max_count << "]");
				return false;
			}
			if(palette_index >= FEATURES.palette.max_count)
			{
				GBGFX_LOG_ERROR(
					"The palette index [" << palette_index
					<< "] is over the palette max count [" << FEATURES.palette.max_count << "]");
				return false;
			}

			const uint32_t bank = tile_index / FEATURES.tileset.tiles_per_bank;
			if(bank >= FEATURES.tileset.bank_max_count)
			{
				GBGFX_LOG_ERROR(
					"The bank index [" << bank
					<< "] is over the bank max count [" << FEATURES.tileset.bank_max_count << "]");
				return false;
			}

			constexpr uint32_t priority = 0;
			out_tilemap.add(
				tile_index % FEATURES.tileset.tiles_per_bank, palette_index, bank,
				flip_type == kTileFlipType_Horizontal || flip_type == kTileFlipType_Both,
				flip_type == kTileFlipType_Vertical || flip_type == kTileFlipType_Both,
				priority);
			return true;
		}))
	{
		return false;
	}

	GBGFX_LOG_INFO(
		"Tilemap size is " << out_tilemap.size()
		<< " in [" << image.getFilename() << "]"); 
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Export
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
	const PaletteSet& palette_set, bool use_header,
	const char* output_filename)
{
	assert(areFeaturesInitialized());
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
	const Tileset& tileset, bool use_header,
	const char* output_filename)
{
	assert(areFeaturesInitialized());
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
	bool use_header, bool use_8800_addressing_mode,
	uint8_t palette_index_offset, uint8_t tile_index_offset,
	const char* indices_filename, const char* parameters_filename)
{
	assert(areFeaturesInitialized());

	if(!FEATURES.tilemap.enabled)
	{
		GBGFX_LOG_ERROR("Exporting tilemaps is not supported in this configuration");
		return false;
	}

	TilemapData data;
	const uint16_t header[2] =
	{
		static_cast<uint8_t>(data.getIndexDataSize()),
		static_cast<uint8_t>(data.getParameterDataSize()),
	};

	if(!data.initialize(
		tilemap, palette_index_offset, tile_index_offset, use_8800_addressing_mode))
	{
		return false;
	}

	if( indices_filename != nullptr && data.getIndexDataSize() > 0)
	{
		if(!writeToFile(
			data.getIndexData(), data.getIndexDataSize(),
			use_header ? &header : nullptr, sizeof(header),
			indices_filename))
		{
			return false;
		}
	}

	if( parameters_filename != nullptr && data.getParameterDataSize() > 0)
	{
		if(!writeToFile(
			data.getParameterData(), data.getParameterDataSize(),
			use_header ? &header : nullptr, sizeof(header),
			parameters_filename))
		{
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Debug
////////////////////////////////////////////////////////////////////////////////

static void blitTile(ColorRGBA* out_pixels, uint32_t pitch, const TileFlip& flip, const Palette& palette)
{
	const uint8_t* indices = flip.color_indices.data();
	for(uint32_t j = 0; j < flip.height; ++j)
	{
		for(uint32_t i = 0; i < flip.width; ++i)
		{
			assert(indices[i] < FEATURES.palette.color_max_count);
			out_pixels[i] = palette[indices[i]];
		}

		indices += flip.width;
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

uint32_t getBasicTileWidth()
{
	assert(areFeaturesInitialized());
	return FEATURES.tileset.basic_tile_width;
}

bool writeTilesetToPNG(
	const Tileset& tileset, TileFlipType flip_type, const PaletteSet& palette_set,
	uint32_t tile_column_count, bool clear_doubles, const char* filename)
{
	assert(areFeaturesInitialized());
	assert(flip_type < kTileFlipType_Count);

	if(tileset.size() == 0)
	{
		GBGFX_LOG_WARN("Skipping output of [" << filename << "] because the tileset is empty");
		return true;
	}

	const TileFlip& base_tile_flip = tileset[0].getTileFlip(flip_type);
	const uint32_t tile_row_count = tileset.size() / tile_column_count + (tileset.size() % tile_column_count == 0 ? 0 : 1);
	const int32_t image_width = static_cast<int32_t>(tile_column_count * base_tile_flip.width);
	const int32_t image_height = static_cast<int32_t>(tile_row_count * base_tile_flip.height);

	TileFlip empty_flip;
	initializeTileFlip(empty_flip, base_tile_flip.width, base_tile_flip.height);
	for(uint32_t i = 0; i < empty_flip.width * empty_flip.height; ++i)
	{
		empty_flip.color_indices[i] = 0;
	}
	Palette empty_palette(true);

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
			pixels + (tile_row * base_tile_flip.height * image_width) + (tile_column * base_tile_flip.width),
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

bool writePaletteSetToPNG(const PaletteSet& palette_set, const char* filename)
{
	assert(areFeaturesInitialized());

	const uint32_t palette_count = palette_set.size();
	if(palette_count == 0)
	{
		return true;
	}

	const uint32_t color_max_count = FEATURES.palette.color_max_count;
	const uint32_t total_color_count = color_max_count * palette_count;
	ColorRGBA* pixels = new ColorRGBA[total_color_count];
	for(uint32_t p = 0; p < palette_count; ++p)
	{
		const Palette& palette = palette_set[p];
		if(palette.size() > color_max_count)
		{
			GBGFX_LOG_ERROR(
				"Palette " << p << " has " << palette.size()
				<< " colors, but the limit for the hardware is "
				<< color_max_count << " colors per palette");
		}
		uint32_t c = 0;
		for(; c < palette.size(); ++c)
		{
			pixels[p * color_max_count + c] = palette[c];
		}
		for(; c < color_max_count; ++c)
		{
			pixels[p * color_max_count + c] = kRGBA_Magenta;
		}
	}

	constexpr int32_t kChannelCount = 4;
	const int result = stbi_write_png(
		filename,
		color_max_count, palette_count, kChannelCount,
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

