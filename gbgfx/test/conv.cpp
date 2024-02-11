#include <cassert>
#include <iostream>

#include "gbgfx.h"

/*
TODO

struct
{
	uint32_t palette_offset_index;
	uint32_t verbosity_level;
	uint32_t palette_max_count;
	bool output_palette;
	bool output_tilemap_parameters;
	bool remove_double_tiles;
	bool use_tile_flips;
	bool use_tile_bank;
	bool use_8800_addressing_mode;
};

DMG
	palette_offset_index		0
	verbosity_level				1
	palette_max_count			1|2
	output_palette				must support
	output_tilemap_parameters	n/a
	remove_double_tiles			supported
	use_tile_flips				n/a
	use_tile_bank				n/a
	use_8800_addressing_mode	supported
	
CGB
	palette_offset_index		0
	verbosity_level				1
	palette_max_count			8
	output_palette				supported
	output_tilemap_parameters	supported
	remove_double_tiles			supported
	use_tile_flips				supported
	use_tile_bank				supported
	use_8800_addressing_mode	supported
*/

int main(int argc, const char** argv)
{
	using namespace gbgfx;

	////////////////////////////////////////

	Tileset tileset;
	PaletteSet palette_set;
	if(!extractTileset(
		tileset, palette_set,
		8, kIterateAllRows, kTileSize, kTileSize,
		false, false, false,
		"test/demo.png"))
	{
		std::cout << "Could not extract tileset" << std::endl;
		return 1;
	}

	Tilemap tilemap;
	if(!extractTilemap(tilemap, tileset, palette_set, false, "test/demo_tlm.png"))
	{
		std::cout << "Could not extract tilemap" << std::endl;
		return 1;
	}

	////////////////////////////////////////

	const bool use_headers = false;

	if(!exportPaletteSet(palette_set, "test/palette.pal", use_headers))
	{
		std::cout << "Could not export palette set" << std::endl;
		return 1;
	}

	if(!exportTileset(tileset, "test/tileset.chr", use_headers))
	{
		std::cout << "Could not export tileset" << std::endl;
		return 1;
	}

	if(!exportTilemap(
		tilemap, "test/tilemap.idx", "test/tilemap.prm",
		use_headers, 0, 128))
	{
		std::cout << "Could not export tilemap" << std::endl;
		return 1;
	}

	////////////////////////////////////////

	for(uint32_t i = 0; i < kTileFlipType_Count; ++i)
	{
		static const char* filenames[] =
		{
			"test/demo_tileset_0.png",
			"test/demo_tileset_1.png",
			"test/demo_tileset_2.png",
			"test/demo_tileset_3.png",
		};
		static_assert(sizeof(filenames) / sizeof(filenames[0]) == kTileFlipType_Count);
		if(!writeTilesetToPNG(filenames[i], 16, tileset, static_cast<TileFlipType>(i), palette_set, i == 0))
		{
			std::cout << "Could not write tileset" << std::endl;
			return 1;
		}
	}

	////////////////////////////////////////

	std::cout << "Done" << std::endl;
	return 0;
}

