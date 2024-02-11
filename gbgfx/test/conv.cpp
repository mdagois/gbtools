#include <cassert>
#include <iostream>

#include "gbgfx.h"

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

	if(!exportPaletteSet(palette_set, "test/palette.pal", false))
	{
		std::cout << "Could not export palette set" << std::endl;
		return 1;
	}

	if(!exportTileset(tileset, "test/tileset.chr", false))
	{
		std::cout << "Could not export tileset" << std::endl;
		return 1;
	}

	if(!exportTilemap(tilemap, "test/tilemap.idx", "test/tilemap.prm", false))
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

