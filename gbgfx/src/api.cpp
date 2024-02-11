#include <cassert>

#include "api.h"
#include "export.h"
#include "image.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////////////////////

static bool readImage(Image& out_image, const char* filename)
{
	return kSuccess == out_image.read(filename);
}

////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	bool use_microtile_8x16, bool remove_doubles, bool remove_flips,
	const char* image_filename)
{
	Image image;
	if(!readImage(image, image_filename))
	{
		return false;
	}

	std::vector<ImageTile> image_tiles;
	if(kSuccess != image.iterateTiles(
		start_tile_row, tile_row_count,
		metatile_width, metatile_height,
		use_microtile_8x16,
		[&image_tiles, &out_palette_set](const ImageTile& tile, uint32_t x, uint32_t y)
		{
			Palette palette;
			if(kSuccess != extractTilePalette(palette, tile))
			{
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
		if(kSuccess != generateTile(tile, image_tiles[i], out_palette_set))
		{
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
	if(!readImage(image, image_filename))
	{
		return false;
	}

	if(!out_tilemap.initialize(image.getHeight() / kTileSize, image.getWidth() / kTileSize))
	{
		return false;
	}
	if(kSuccess != image.iterateTiles(
		[&out_tilemap, &tileset, &palette_set, use_flips](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Tile tile;
			if(kSuccess != generateTile(tile, image_tile, palette_set))
			{
				return false;
			}

			uint32_t tile_index;
			uint32_t palette_index;
			TileFlipType flip_type;
			if(!tileset.findTileIndex(tile_index, palette_index, flip_type, tile, use_flips))
			{
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
		}))
	{
		return false;
	}

	return true;
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

