#include <cassert>
#include <iostream>

#include "gbgfx.h"

static bool readImage(Image& out_image, const char* filename)
{
	if(kSuccess != out_image.read(filename))
	{
		std::cout
			<< "Cannot read file [" << filename << "]"
			<< std::endl;
		return false;
	}
	std::cout
		<< "Loaded [" << filename << "] "
		<< out_image.getWidth() << "x" << out_image.getHeight()
		<< std::endl;
	return true;
}

static bool extractTileset(
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

	out_palette_set.optimize();

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

static bool extractTilemap(
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

	Tilemap tilemap;
	if(!extractTilemap(tilemap, tileset, palette_set, false, "test/demo_tlm.png"))
	{
		cout << "Could not extract tilemap" << endl;
		return 1;
	}

	return 0;
}

