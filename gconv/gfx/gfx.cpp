#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

#include "third_party/stb_image_write.h"

#include "gfx.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////

bool initialize(Hardware hardware)
{
	assert(!areCapabilitiesInitialized());
	return initializeCapabilities(hardware);
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
	if(colors.size() > CAPS.palette.color_max_count)
	{
		GFX_LOG_ERROR("Too many colors in image tile");
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
	TileFlip& out_tile_flip, const ImageTile& image_tile, const Palette& palette)
{
	initializeTileFlip(out_tile_flip, image_tile.getWidth(), image_tile.getHeight());

	const uint32_t pixel_count = image_tile.getWidth() * image_tile.getHeight();
	for(uint32_t i = 0; i < pixel_count; ++i)
	{
		uint8_t color_index = 0;
		if(!palette.findColorIndex(color_index, image_tile[i]))
		{
			GFX_LOG_ERROR("Could not find color in palette");
			return false;
		}
		out_tile_flip.color_indices[i] = color_index;
	}
	return true;
}

static bool generateTile(Tile& out_tile, const ImageTile& image_tile, const Palette& palette)
{
	TileFlip tile_flip;
	if(!generateTileFlip(tile_flip, image_tile, palette))
	{
		GFX_LOG_ERROR("Could not generate tile flip");
		return false;
	}
	out_tile.initializeFlips(tile_flip);
	return true;
}

static bool addBasicTileSize(std::vector<Division>& divisions)
{
	if(divisions.empty())
	{
		divisions.push_back({ CAPS.tileset.basic_tile_width, CAPS.tileset.basic_tile_height, false });
		return true;
	}
	const Division& last = divisions.back();
	if(last.width == CAPS.tileset.basic_tile_width && last.height == CAPS.tileset.basic_tile_height)
	{
		return true;
	}
	if(last.width < CAPS.tileset.basic_tile_width || last.height < CAPS.tileset.basic_tile_height)
	{
		GFX_LOG_ERROR(
			"The last division " << last.width << "x" << last.height
			<< " is smaller than the basic tile "
			<< CAPS.tileset.basic_tile_width << "x" << CAPS.tileset.basic_tile_height);
		return false;
	}
	divisions.push_back({ CAPS.tileset.basic_tile_width, CAPS.tileset.basic_tile_height, false });
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Import
////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set, DivisionInfo& out_division_info,
	const std::vector<Division>& divisions, const gfx::Rectangle rectangle,
	TileRemoval tile_removal, const char* image_filename)
{
	assert(areCapabilitiesInitialized());
	Image image;
	if(!image.read(image_filename))
	{
		return false;
	}
	return extractTileset(
		out_tileset, out_palette_set, out_division_info,
		divisions, rectangle, tile_removal, image);
}

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set, DivisionInfo& out_division_info,
	const std::vector<Division>& divisions, const gfx::Rectangle rectangle,
	TileRemoval tile_removal, const Image& image)
{
	assert(areCapabilitiesInitialized());

	////////////////////////////////////////////////////////////
	// Prepare divisions.
	////////////////////////////////////////////////////////////

	std::vector<Division> final_divisions = divisions;
	if(!addBasicTileSize(final_divisions))
	{
		return false;
	}

	////////////////////////////////////////////////////////////
	// Extract palettes and build palette set.
	////////////////////////////////////////////////////////////

	if(out_palette_set.isLocked())
	{
		for(uint32_t i = 0; i < out_palette_set.size(); ++i)
		{
			if(out_palette_set[i].size() > CAPS.palette.color_max_count)
			{
				GFX_LOG_ERROR(
					"Palette #" << i <<  " in provided palette set has too many colors ("
					<< out_palette_set[i].size() << " > " << CAPS.palette.color_max_count
					<< ") in [" << image.getFilename() << "]");
				return false;
			}
		}
	}
	else
	{
		if(!image.iterateTiles(
			out_division_info,
			final_divisions.data(), static_cast<uint32_t>(final_divisions.size()),
			rectangle,
			[&out_palette_set, &image](const ImageTile& image_tile, uint32_t x, uint32_t y)
			{
				Palette palette(CAPS.palette.insert_transparent_color);
				if(!extractTilePalette(palette, image_tile))
				{
					GFX_LOG_ERROR(
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

		if(!out_palette_set.optimize(CAPS.palette.color_max_count, CAPS.palette.share_first_color, true))
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////
	// Compute the palette index for each tile.
	////////////////////////////////////////////////////////////

	struct TileMetadata
	{
		std::set<uint32_t> compatible_palettes;
	};
	std::vector<TileMetadata> metadata_buffer;
	if(!image.iterateTiles(
		out_division_info,
		final_divisions.data(), static_cast<uint32_t>(final_divisions.size()),
		rectangle,
		[&out_tileset, &out_palette_set, &image, &metadata_buffer](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Palette tile_palette(CAPS.palette.insert_transparent_color);
			if(!extractTilePalette(tile_palette, image_tile))
			{
				GFX_LOG_ERROR("Could not extract palette");
				return false;
			}
			TileMetadata& metadata = metadata_buffer.emplace_back();
			for(uint32_t p = 0; p < out_palette_set.size(); ++p)
			{
				const Palette& palette = out_palette_set[p];
				if(palette.contains(tile_palette))
				{
					metadata.compatible_palettes.insert(p);
				}
			}

			return true;
		}))
	{
		return false;
	}

	if(metadata_buffer.size() % CAPS.tileset.tile_group_size != 0)
	{
		GFX_LOG_ERROR(
			"The number of tiles (" << metadata_buffer.size()
			<< ") is not multiple of the tile group size ("
			<< CAPS.tileset.tile_group_size << ")");
		return false;
	}

	auto computeIntersection = [](std::set<uint32_t>& lhs, std::set<uint32_t>& rhs)
	{
		std::set<uint32_t> result;
		for(uint32_t palette_index : lhs)
		{
			if(rhs.find(palette_index) != rhs.end())
			{
				result.insert(palette_index);
			}
		}
		return result;
	};

	std::vector<uint32_t> tile_palette_index;
	for(uint32_t i = 0; i < metadata_buffer.size(); i += CAPS.tileset.tile_group_size)
	{
		std::set<uint32_t> palettes = metadata_buffer[i].compatible_palettes;
		for(uint32_t j = 1; j < CAPS.tileset.tile_group_size; ++j)
		{
			palettes = computeIntersection(palettes, metadata_buffer[i + j].compatible_palettes);
		}
		if(palettes.size() == 0)
		{
			GFX_LOG_ERROR(
				"Could not find a common palette for tile_group ("
				<< (i / CAPS.tileset.tile_group_size) << ")");
			return false;
		}
		for(uint32_t j = 0; j < CAPS.tileset.tile_group_size; ++j)
		{
			tile_palette_index.push_back(*palettes.cbegin());
		}
	}

	////////////////////////////////////////////////////////////
	// Generate all the tiles in the tileset.
	////////////////////////////////////////////////////////////

	uint32_t tile_index = 0;
	if(!image.iterateTiles(
		out_division_info,
		final_divisions.data(), static_cast<uint32_t>(final_divisions.size()),
		rectangle,
		[&out_tileset, &out_palette_set, &image, &tile_index, &tile_palette_index](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			const uint32_t palette_index = tile_palette_index[tile_index];
			Tile tile;
			tile.setPaletteIndex(palette_index);
			if(!generateTile(tile, image_tile, out_palette_set[palette_index]))
			{
				GFX_LOG_ERROR(
					"Could not generate tile ("
					<< x << "," << y << ") in [" << image.getFilename() << "]");
				return false;
			}
			out_tileset.add(tile);
			++tile_index;
			return true;
		}))
	{
		return false;
	}

	////////////////////////////////////////////////////////////
	// Tileset optimization.
	////////////////////////////////////////////////////////////

	if(tile_removal != kTileRemovalNone)
	{
		if(CAPS.tileset.supports_tile_removal)
		{
			const bool has_flips =
				(CAPS.tilemap.enabled && CAPS.tilemap.supports_tile_flips) ||
				(CAPS.sprite.enabled && CAPS.sprite.supports_tile_flips);
			if(!has_flips && tile_removal == kTileRemovalFlips)
			{
				GFX_LOG_WARN(
					"Downgraded tile removal from " << tile_removal
					<< " to " << kTileRemovalDoubles
					<< " as the hardware does not support flips");
				tile_removal = kTileRemovalDoubles;
			}
			out_tileset.removeDoubles(tile_removal == kTileRemovalFlips);
		}
		else
		{
			GFX_LOG_WARN(
				"Ignored tile removal mode " << tile_removal
				<< " as the hardware does not support tile removal");
		}
	}

	////////////////////////////////////////////////////////////
	// Print information.
	////////////////////////////////////////////////////////////

	GFX_LOG_INFO(
		"Tile count is " << out_tileset.size()
		<< " and palette count is " << out_palette_set.size()
		<< " in [" << image.getFilename() << "]"); 
	if(out_tileset.size() > CAPS.tileset.tile_max_count)
	{
		GFX_LOG_INFO(
			"The tile count " << out_tileset.size()
			<< " is over the maximum of " << CAPS.tileset.tile_max_count);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool extractTilemap(
	Tilemap& out_tilemap, DivisionInfo& out_division_info,
	const Tileset& tileset, const PaletteSet& palette_set,
	const std::vector<Division>& divisions, const gfx::Rectangle rectangle,
	const char* image_filename)
{
	assert(areCapabilitiesInitialized());
	Image image;
	if(!image.read(image_filename))
	{
		return false;
	}
	return extractTilemap(out_tilemap, out_division_info, tileset, palette_set, divisions, rectangle, image);
}

bool extractTilemap(
	Tilemap& out_tilemap, DivisionInfo& out_division_info,
	const Tileset& tileset, const PaletteSet& palette_set,
	const std::vector<Division>& divisions, const gfx::Rectangle rectangle,
	const Image& image)
{
	assert(areCapabilitiesInitialized());

	////////////////////////////////////////////////////////////
	// Prepare divisions.
	////////////////////////////////////////////////////////////

	std::vector<Division> final_divisions = divisions;
	if(!addBasicTileSize(final_divisions))
	{
		return false;
	}

	////////////////////////////////////////////////////////////
	// Generate all the tiles in the tileset.
	////////////////////////////////////////////////////////////

	if(!image.iterateTiles(
		out_division_info,
		final_divisions.data(), static_cast<uint32_t>(final_divisions.size()),
		rectangle,
		[&out_tilemap, &tileset, &palette_set, &image](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Palette tile_palette(CAPS.palette.insert_transparent_color);
			if(!extractTilePalette(tile_palette, image_tile))
			{
				GFX_LOG_ERROR(
					"Could not extract palette for tile ("
					<< x << "," << y << ") in [" << image.getFilename() << "]");
				return false;
			}

			uint32_t palette_index = kInvalidIndex;
			if(!palette_set.findCompatiblePaletteIndex(palette_index, tile_palette))
			{
				GFX_LOG_ERROR(
					"Could not find a compatible palette for tile ("
					<< x << "," << y << ") in [" << image.getFilename() << "]");
				return false;
			}

			Tile tile;
			tile.setPaletteIndex(palette_index);
			if(!generateTile(tile, image_tile, palette_set[palette_index]))
			{
				GFX_LOG_ERROR(
					"Could not generate tile ("
					<< x << "," << y << ") in [" << image.getFilename() << "]");
				return false;
			}

			uint32_t tile_index;
			TileFlipType flip_type;
			if(!tileset.findTileIndex(
				tile_index, flip_type, tile,
				CAPS.tilemap.supports_tile_flips))
			{
				GFX_LOG_ERROR(
					"Could not find tile ("
					<< x << "," << y << ") in tileset from [" << image.getFilename() << "]");
				return false;
			}

			if(tile_index >= CAPS.tileset.tile_max_count)
			{
				GFX_LOG_ERROR(
					"The tile index [" << tile_index
					<< "] is over the tile max count [" << CAPS.tileset.tile_max_count << "]");
				return false;
			}
			if(palette_index >= CAPS.palette.max_count)
			{
				GFX_LOG_ERROR(
					"The palette index [" << palette_index
					<< "] is over the palette max count [" << CAPS.palette.max_count << "]");
				return false;
			}

			const uint32_t bank = tile_index / CAPS.tileset.tiles_per_bank;
			if(bank >= CAPS.tileset.bank_max_count)
			{
				GFX_LOG_ERROR(
					"The bank index [" << bank
					<< "] is over the bank max count [" << CAPS.tileset.bank_max_count << "]");
				return false;
			}

			constexpr uint32_t priority = 0;
			out_tilemap.add(
				tile_index % CAPS.tileset.tiles_per_bank, palette_index, bank,
				flip_type == kTileFlipType_Horizontal || flip_type == kTileFlipType_Both,
				flip_type == kTileFlipType_Vertical || flip_type == kTileFlipType_Both,
				priority);
			return true;
		}))
	{
		return false;
	}

	////////////////////////////////////////////////////////////
	// Print information.
	////////////////////////////////////////////////////////////

	GFX_LOG_INFO(
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
		GFX_LOG_ERROR("Could not open file [" << output_filename << "]");
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
		GFX_LOG_ERROR("Could not write file [" << output_filename << "]");
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool exportPaletteSet(
	const PaletteSet& palette_set, bool use_header,
	const char* output_filename)
{
	assert(areCapabilitiesInitialized());
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
	assert(areCapabilitiesInitialized());
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
	assert(areCapabilitiesInitialized());

	if(!CAPS.tilemap.enabled)
	{
		GFX_LOG_ERROR("Exporting tilemaps is not supported in this configuration");
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
// Info
////////////////////////////////////////////////////////////////////////////////

static bool loadFile(uint8_t*& out_data, uint32_t& out_data_size, const char* filename)
{
	assert(filename != nullptr);

	FILE* file = fopen(filename, "rb");
	if(file == nullptr)
	{
		GFX_LOG_ERROR("Could not open file [" << filename << "] for read");
		return false;
	}

	fseek(file, 0, SEEK_END);
	const long int length = ftell(file);
	if(length <= 0)
	{
		GFX_LOG_ERROR("File size if zero [" << filename << "]");
		fclose(file);
		return false;
	}
	fseek(file, 0, SEEK_SET);

	uint8_t* data = new uint8_t[length];
	if(data == nullptr)
	{
		GFX_LOG_ERROR("Could not allocate enough memory [" << length << "] for [" << filename << "]");
		fclose(file);
		return false;
	}

	size_t n = fread(data, length, 1, file);
	fclose(file);
	if(n != 1)
	{
		GFX_LOG_ERROR("Could not read the data for file [" << filename << "]");
		delete [] data;
		return false;
	}

	out_data = data;
	out_data_size = length;
	return true;
}

bool loadDivisionInfo(DivisionInfo& out_division_info, const char* input_filename)
{
	assert(input_filename != nullptr);

	uint8_t* data = nullptr;
	uint32_t data_size = 0;
	if(!loadFile(data, data_size, input_filename))
	{
		return false;
	}

	const uint32_t* data32 = reinterpret_cast<const uint32_t*>(data);
	out_division_info.clear();
	out_division_info.image_width = *data32;
	++data32;
	out_division_info.image_height = *data32;
	++data32;
	const uint32_t list_count = *data32;
	++data32;
	out_division_info.resize(list_count);
	for(uint32_t i = 0; i < list_count; ++i)
	{
		DivisionStatusList& list = out_division_info[i];
		list.division.width = *data32;
		++data32;
		list.division.height = *data32;
		++data32;
		list.division.skip_transparent = *data32 != 0;
		++data32;
		const uint32_t list_size = *data32;
		++data32;
		list.resize(list_size);
		const DivisionStatus* list_data = reinterpret_cast<const DivisionStatus*>(data32);
		memcpy(list.data(), list_data, sizeof(DivisionStatus) * list_size);
		data32 = reinterpret_cast<const uint32_t*>(list_data + (sizeof(DivisionStatus) * list_size));
	}

	return computeDivisionInfoOffsets(out_division_info);
}

bool writeDivisionInfo(const DivisionInfo& division_info, const char* output_filename)
{
	assert(output_filename != nullptr);

	FILE* output_file = fopen(output_filename, "wb");
	if(output_file == nullptr)
	{
		GFX_LOG_ERROR("Could not open file [" << output_filename << "]");
		return false;
	}
	assert(output_file != nullptr);

	const uint32_t info_header[] =
	{
		division_info.image_width,
		division_info.image_height,
		static_cast<uint32_t>(division_info.size()),
	};
	if(fwrite(info_header, sizeof(info_header), 1, output_file) != 1)
	{
		GFX_LOG_ERROR("Could not write header for file [" << output_filename << "]");
		return false;
	}

	for(const DivisionStatusList& list :  division_info)
	{
		const uint32_t row = division_info.image_height / list.division.height;
		const uint32_t column = division_info.image_width / list.division.width;
		assert(list.size() == row * column);
		const uint32_t header[] =
		{
			list.division.width,
			list.division.height,
			list.division.skip_transparent ? 1U : 0U,
			static_cast<uint32_t>(list.size()),
		};
		if(fwrite(header, sizeof(header), 1, output_file) != 1)
		{
			GFX_LOG_ERROR("Could not write division header for file [" << output_filename << "]");
			return false;
		}
		if(fwrite(list.data(), sizeof(DivisionStatus) * list.size(), 1, output_file) != 1)
		{
			GFX_LOG_ERROR("Could not write division data for file [" << output_filename << "]");
			return false;
		}
	}

	fclose(output_file);
	return true;
}

bool printDivisionInfo(const DivisionInfo& info)
{
	std::cout << info.image_width << "x" << info.image_height << std::endl;
	for(size_t i = 0; i < info.size(); ++i)
	{
		auto& list = info[i];
		std::cout
			<< list.division.width << "x" << list.division.height
			<< (list.division.skip_transparent ? 's' : 'k') << std::endl;
		const uint32_t row_count = info.image_height / list.division.height;
		const uint32_t col_count = info.image_width / list.division.width;
		for(uint32_t row = 0; row < row_count; ++row)
		{
			for(uint32_t col = 0; col < col_count; ++col)
			{
				std::cout << getLetterFromStatus(list[row * col_count + col]);
			}
			std::cout << std::endl;
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
			assert(indices[i] < CAPS.palette.color_max_count);
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
	assert(areCapabilitiesInitialized());
	return CAPS.tileset.basic_tile_width;
}

bool writeTilesetToPNG(
	const Tileset& tileset, TileFlipType flip_type, const PaletteSet& palette_set,
	uint32_t tile_column_count, bool clear_doubles, const char* filename)
{
	assert(areCapabilitiesInitialized());
	assert(flip_type < kTileFlipType_Count);

	if(tileset.size() == 0)
	{
		GFX_LOG_WARN("Skipping output of [" << filename << "] because the tileset is empty");
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
		GFX_LOG_ERROR("Could not write file [" << filename << "]");
		return false;
	}
	return true;
}

bool writePaletteSetToPNG(const PaletteSet& palette_set, const char* filename)
{
	assert(areCapabilitiesInitialized());

	const uint32_t palette_count = palette_set.size();
	if(palette_count == 0)
	{
		return true;
	}

	const uint32_t color_max_count = CAPS.palette.color_max_count;
	const uint32_t total_color_count = color_max_count * palette_count;
	ColorRGBA* pixels = new ColorRGBA[total_color_count];
	for(uint32_t p = 0; p < palette_count; ++p)
	{
		const Palette& palette = palette_set[p];
		if(palette.size() > color_max_count)
		{
			GFX_LOG_ERROR(
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
		GFX_LOG_ERROR("Could not write file [" << filename << "]");
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

}

