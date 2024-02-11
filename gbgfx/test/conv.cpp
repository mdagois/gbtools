#include <cassert>
#include <iostream>

#include "gbgfx.h"

int main(int argc, const char** argv)
{
	using namespace std;

	Tileset tileset;
	PaletteSet palette_set;
	if(!extractTileset(
		tileset, palette_set,
		8, kIterateAllRows, kTileSize, kTileSize,
		false, false, false,
		"test/demo.png"))
	{
		cout << "Could not extract tileset" << endl;
		return 1;
	}

	cout << "Tile count: " << tileset.size() << endl;
	cout << "Palette count: " << palette_set.size() << endl;

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
		if(kSuccess != writeTilesetToPNG(filenames[i], 16, tileset, static_cast<TileFlipType>(i), palette_set, i == 0))
		{
			cout << "Could not write tileset" << endl;
			return 1;
		}
	}

	if(!exportPaletteSet(palette_set, "test/palette.pal"))
	{
		cout << "Could not export palette set" << endl;
		return 1;
	}

	if(!exportTileset(tileset, "test/tileset.chr"))
	{
		cout << "Could not export tileset" << endl;
		return 1;
	}

	Tilemap tilemap;
	if(!extractTilemap(tilemap, tileset, palette_set, false, "test/demo_tlm.png"))
	{
		cout << "Could not extract tilemap" << endl;
		return 1;
	}
	cout << "Tilemap size " << tilemap.getColumnCount() << "x" << tilemap.getRowCount() << endl;

	if(!exportTilemap(tilemap, "test/tilemap.idx", "test/tilemap.prm"))
	{
		cout << "Could not export tilemap" << endl;
		return 1;
	}

	cout << "Done" << endl;
	return 0;
}

