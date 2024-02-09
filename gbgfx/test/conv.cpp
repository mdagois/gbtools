#include <cassert>
#include <iostream>
#include "gbgfx.h"

using namespace std;

int main(int argc, const char** argv)
{
	const char* image_filename = "test/demo.png";

	Image image;
	if(kSuccess != image.read(image_filename))
	{
		cout << "Cannot read file" << endl;
		return 1;
	}
	cout
		<< "Loaded [" << image_filename << "] "
		<< image.getWidth() << "x" << image.getHeight()
		<< endl;

	std::vector<ImageTile> image_tiles;
	PaletteSet palette_set;
	uint32_t tile_count = 0;
	if(kSuccess != image.iterateTiles(
#if 1
		0, kIterateAllRows, kTileSize, kTileSize,
		false,
#else
		0, kIterateAllRows, kTileSize * 2, kTileSize * 2,
		true,
#endif
		[&image_tiles, &palette_set](const ImageTile& tile, uint32_t x, uint32_t y)
		{
			Palette palette;
			if(kSuccess != extractTilePalette(palette, tile))
			{
				assert(false);
			}
			palette_set.push(palette);
			image_tiles.push_back(tile);
			return true;
		}))
	{
		cout << "Error on tile" << endl;
		return 1;
	}

	Tileset tileset;
	for(uint32_t i = 0; i < image_tiles.size(); ++i)
	{
		Tile tile;
		if(kSuccess != generateTile(tile, image_tiles[i], palette_set))
		{
			cout << "Cannot generate tile (" << i << ") from image tile and palette set" << endl;
			return 1;
		}
		tileset.push(tile);
	}

	cout << "Tile count: " << tileset.size() << endl;
	cout << "Palette count: " << palette_set.size() << endl;
	for(uint32_t i = 0; i < palette_set.size(); ++i)
	{
		cout << "\tsize = " << palette_set[i].size() << endl;
	}
	palette_set.optimize();
	cout << "Palette count: " << palette_set.size() << endl;
	for(uint32_t i = 0; i < palette_set.size(); ++i)
	{
		cout << "\tsize = " << palette_set[i].size() << endl;
	}

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

	{
		TilesetData tileset_data;
		tileset_data.initialize(tileset);
		FILE* file = fopen("test/tileset.chr", "wb");
		fwrite(tileset_data.getData(), tileset_data.getDataSize(), 1, file);
		fclose(file);
	}

	tileset.removeDoubles(false);
	cout << "Tile count: " << tileset.size() << endl;
	if(kSuccess != writeTilesetToPNG("test/demo_tileset_opt_0.png", 16, tileset, kTileFlipType_None, palette_set, true))
	{
		cout << "Could not write tileset" << endl;
		return 1;
	}

	tileset.removeDoubles(true);
	cout << "Tile count: " << tileset.size() << endl;
	if(kSuccess != writeTilesetToPNG("test/demo_tileset_opt_1.png", 16, tileset, kTileFlipType_None, palette_set, true))
	{
		cout << "Could not write tileset" << endl;
		return 1;
	}

	return 0;
}

