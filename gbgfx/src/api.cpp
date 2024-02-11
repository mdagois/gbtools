#include <cassert>

#include "api.h"
#include "export.h"
#include "image.h"
#include "log.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	bool use_microtile_8x16, bool remove_doubles, bool remove_flips,
	const char* image_filename)
{
	Image image;
	if(!image.read(image_filename))
	{
		return false;
	}

	std::vector<ImageTile> image_tiles;
	if(!image.iterateTiles(
		start_tile_row, tile_row_count,
		metatile_width, metatile_height,
		use_microtile_8x16,
		[&image_tiles, &out_palette_set, image_filename](const ImageTile& tile, uint32_t x, uint32_t y)
		{
			Palette palette;
			if(!extractTilePalette(palette, tile))
			{
				LOG_ERROR(
					"Could not extract palette from metatile ("
					<< x << "," << y << ") in [" << image_filename << "]");
				return false;
			}
			out_palette_set.push(palette);
			image_tiles.push_back(tile);
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
			LOG_ERROR(
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
		LOG_ERROR("Could not initialize tilemap from [" << image_filename << "]");
		return false;
	}
	return image.iterateTiles(
		[&out_tilemap, &tileset, &palette_set, use_flips, image_filename](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Tile tile;
			if(!generateTile(tile, image_tile, palette_set))
			{
				LOG_ERROR(
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
				LOG_ERROR(
					"Could not find tile ("
					<< x << "," << y << ") in tileset in ["
					<< image_filename << "]");
				return false;
			}

			assert(tile_index < 512);
			constexpr uint32_t priority = 0;
			const uint32_t bank = tile_index < 256 ? 0 : 1;
			out_tilemap.push(
				tile_index, palette_index, bank,
				flip_type == kTileFlipType_Horizontal || flip_type == kTileFlipType_Both,
				flip_type == kTileFlipType_Vertical || flip_type == kTileFlipType_Both,
				priority);
			return true;
		});
}

////////////////////////////////////////////////////////////////////////////////
// Output
////////////////////////////////////////////////////////////////////////////////

static bool writeToFile(const void* data, uint32_t size, const char* output_filename)
{
	assert(output_filename != nullptr);

	FILE* output_file = fopen(output_filename, "wb");
	if(output_file == nullptr)
	{
		LOG_ERROR("Could not open file [" << output_filename << "]");
		return false;
	}
	assert(output_file != nullptr);

	if(size == 0)
	{
		fclose(output_file);
	}

	const size_t written = fwrite(data, size, 1, output_file);
	fclose(output_file);
	if(written != 1)
	{
		LOG_ERROR("Could not write file [" << output_filename << "]");
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool exportPaletteSet(const PaletteSet& palette_set, const char* output_filename)
{
	PaletteSetData data;
	return
		data.initialize(palette_set) &&
		writeToFile(data.getData(), data.getDataSize(), output_filename);
}

////////////////////////////////////////////////////////////////////////////////

bool exportTileset(const Tileset& tileset, const char* output_filename)
{
	TilesetData data;
	return
		data.initialize(tileset) &&
		writeToFile(data.getData(), data.getDataSize(), output_filename);
}

////////////////////////////////////////////////////////////////////////////////

bool exportTilemap(const Tilemap& tilemap, const char* indices_filename, const char* parameter_filename)
{
	TilemapData data;
	return
		data.initialize(tilemap) &&
		(indices_filename == nullptr || writeToFile(data.getIndexData(), data.getDataSize(), indices_filename)) &&
		(parameter_filename == nullptr || writeToFile(data.getParameterData(), data.getDataSize(), parameter_filename));
}
