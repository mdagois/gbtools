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
	if(kSuccess != image.iterateTiles(
#if 1
		8, kIterateAllRows, kTileSize, kTileSize,
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
		PaletteSetData palette_set_data;
		palette_set_data.initialize(palette_set);
		FILE* file = fopen("test/palette.pal", "wb");
		fwrite(palette_set_data.getData(), palette_set_data.getDataSize(), 1, file);
		fclose(file);
	}

	{
		TilesetData tileset_data;
		tileset_data.initialize(tileset);
		FILE* file = fopen("test/tileset.chr", "wb");
		fwrite(tileset_data.getData(), tileset_data.getDataSize(), 1, file);
		fclose(file);
	}

	cout << "Tile count: " << tileset.size() << endl;

	tileset.removeDoubles(false);
	cout << "Tile count: " << tileset.size() << endl;
	if(kSuccess != writeTilesetToPNG("test/demo_tileset_opt_0.png", 16, tileset, kTileFlipType_None, palette_set, true))
	{
		cout << "Could not write tileset" << endl;
		return 1;
	}

#if 0
	tileset.removeDoubles(true);
	cout << "Tile count: " << tileset.size() << endl;
	if(kSuccess != writeTilesetToPNG("test/demo_tileset_opt_1.png", 16, tileset, kTileFlipType_None, palette_set, true))
	{
		cout << "Could not write tileset" << endl;
		return 1;
	}
#endif

	image_filename = "test/demo_tlm.png";
	if(kSuccess != image.read(image_filename))
	{
		cout << "Cannot read file" << endl;
		return 1;
	}
	cout
		<< "Loaded [" << image_filename << "] "
		<< image.getWidth() << "x" << image.getHeight()
		<< endl;

	Tilemap tilemap;
	tilemap.initialize(image.getHeight() / kTileSize, image.getWidth() / kTileSize);
	if(kSuccess != image.iterateTiles(
		[&tilemap, &tileset, &palette_set](const ImageTile& image_tile, uint32_t x, uint32_t y)
		{
			Tile tile;
			if(kSuccess != generateTile(tile, image_tile, palette_set))
			{
				cout << "Cannot generate tile (" << x << "," << y << ") from image tile and palette set" << endl;
				return false;
			}

			uint32_t tile_index;
			uint32_t palette_index;
			TileFlipType flip_type;
			if(!tileset.findTileIndex(tile_index, palette_index, flip_type, tile, false))
			{
				return false;
			}

			assert(tile_index < 512);
			constexpr uint32_t priority = 0;
			const uint32_t bank = tile_index < 256 ? 0 : 1;
			tilemap.push(
				tile_index, palette_index, bank,
				flip_type == kTileFlipType_Horizontal || flip_type == kTileFlipType_Both,
				flip_type == kTileFlipType_Vertical || flip_type == kTileFlipType_Both,
				priority);
			return true;
		}))
	{
		cout << "Error on tilemap" << endl;
		return 1;
	}

	return 0;
}

