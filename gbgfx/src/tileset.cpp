#pragma once

#include "tileset.h"

Tileset::Tileset()
{
}

Tileset::~Tileset()
{
}

void Tileset::push(const Tile& tile)
{
}

uint32_t Tileset::size() const
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

//enum TileFlipType : uint32_t
//{
//	kTileFlipType_None,
//	kTileFlipType_Horizontal,
//	kTileFlipType_Vertical,
//	kTileFlipType_Both,
//	kTileFlipType_Count,
//};
//
//struct TileFlip
//{
//	union
//	{
//		uint16_t rows[kTileSize];
//		uint64_t values[kTileSize * sizeof(uint16_t) / sizeof(uint64_t)];
//	};
//
//	bool operator<(const TileFlip& other) const
//	{
//		return tie(values[0], values[1]) < tie(other.values[0], other.values[1]);
//	}
//};
//
//struct Tile
//{
//	TileFlip flips[kTileFlipType_Count];
//	uint32_t palette_index;
//};
//
//struct TileFlipIndex
//{
//	size_t index;
//	TileFlipType flip_type;
//};
//
//struct Tileset
//{
//	vector<Tile> tiles;
//	map<TileFlip, TileFlipIndex> flipToIndex;
//};
//
//static void generateTileFlips(Tile& out_tile)
//{
//	const TileFlip& none = out_tile.flips[kTileFlipType_None];
//	{
//		TileFlip& horizontal = out_tile.flips[kTileFlipType_Horizontal];
//		for(uint32_t i = 0; i < kTileSize; ++i)
//		{
//			const uint8_t* src_bytes = reinterpret_cast<const uint8_t*>(none.rows + i);
//			uint8_t* dst_bytes = reinterpret_cast<uint8_t*>(horizontal.rows + i);
//			for(uint32_t b = 0; b < 2; ++b)
//			{
//				dst_bytes[b] =
//					((src_bytes[b] & 0x01) << 7) |
//					((src_bytes[b] & 0x02) << 5) |
//					((src_bytes[b] & 0x04) << 3) |
//					((src_bytes[b] & 0x08) << 1) |
//					((src_bytes[b] & 0x10) >> 1) |
//					((src_bytes[b] & 0x20) >> 3) |
//					((src_bytes[b] & 0x40) >> 5) |
//					((src_bytes[b] & 0x80) >> 7);
//			}
//		}
//	}
//	{
//		TileFlip& vertical = out_tile.flips[kTileFlipType_Vertical];
//		for(uint32_t i = 0; i < kTileSize; ++i)
//		{
//			vertical.rows[(kTileSize - 1) - i] = none.rows[i];
//		}
//	}
//	{
//		TileFlip& both = out_tile.flips[kTileFlipType_Both];
//		const TileFlip& horizontal = out_tile.flips[kTileFlipType_Horizontal];
//		for(uint32_t i = 0; i < kTileSize; ++i)
//		{
//			both.rows[(kTileSize - 1) - i] = horizontal.rows[i];
//		}
//	}
//}

//static bool findMatchingPaletteInSet(uint32_t& out_palette_index, const PaletteSet& palette_set, const Palette& palette)
//{
//	for(uint32_t i = 0; i < kPaletteMaxCount; ++i)
//	{
//		if(isSuperPalette(palette_set.palettes[i], palette))
//		{
//			out_palette_index = i;
//			return true;
//		}
//	}
//	return false;
//}

//static bool extractTile(Tile& out_tile, const PaletteSet& palette_set, const ColorRGBA* pixels, uint32_t row_pitch)
//{
//	Palette tile_palette;
//	{
//		Palette palette;
//		if(!extractTilePalette(palette, pixels, row_pitch))
//		{
//			cout << "Could not extract tile color" << endl;
//			return false;
//		}
//		uint32_t palette_index;
//		if(!findMatchingPaletteInSet(palette_index, palette_set, palette))
//		{
//			cout << "Could not find a matching palette in the palette set" << endl;
//			return false;
//		}
//		tile_palette = palette_set.palettes[palette_index];
//		out_tile.palette_index = palette_index;
//	}
//
//	auto getColorIndex = [&tile_palette](ColorBGR555 color)
//	{
//		for(uint32_t i = 0; i < kColorsPerPalette; ++i)
//		{
//			if(color == tile_palette.colors[i])
//			{
//				return i;
//			}
//		}
//		return static_cast<uint32_t>(kColorIndex_Invalid);
//	};
//
//	TileFlip& none = out_tile.flips[kTileFlipType_None];
//	for(uint32_t j = 0; j < kTileSize; ++j)
//	{
//		uint16_t& row_value = none.rows[j];
//		row_value = 0;
//		uint8_t* bytes = reinterpret_cast<uint8_t*>(&row_value);
//
//		const ColorRGBA* row_pixels = pixels + (j * row_pitch);
//		for(uint32_t i = 0; i < kTileSize; ++i)
//		{
//			const ColorBGR555 color = convertColor(row_pixels[i]);
//			const size_t color_index = getColorIndex(color);
//			if(color_index == kColorIndex_Invalid)
//			{
//				cout << "The color referenced by the tile was not found in the tile palette" << endl;
//				return false;
//			}
//			assert(color_index < kColorsPerPalette);
//			const uint32_t shift = (kTileSize - 1) - i;
//			bytes[0] |= (color_index & 0x1) << shift;
//			bytes[1] |= ((color_index >> 1) & 0x1) << shift;
//		}
//	}
//
//	generateTileFlips(out_tile);
//	return true;
//}

//static bool extractTileset(Tileset& out_tileset, const PaletteSet& palette_set, const Image& image, bool optimize_tileset)
//{
//	const bool success = iterateImageTiles(
//		image,
//		[&image, &palette_set, optimize_tileset, &out_tileset](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
//		{
//			Tile tile;
//			if(!extractTile(tile, palette_set, tile_pixels, image.getWidth()))
//			{
//				cout << "Could not extract tile (" << tile_column << "," << tile_row << ")" << endl;
//				return false;
//			}
//			if(optimize_tileset && out_tileset.flipToIndex.find(tile.flips[kTileFlipType_None]) != out_tileset.flipToIndex.end())
//			{
//				return true;
//			}
//
//			const size_t tile_index = out_tileset.tiles.size();
//			for(uint32_t i = 0; i < kTileFlipType_Count; ++i)
//			{
//				TileFlipIndex flip_index = {};
//				flip_index.index = tile_index;
//				flip_index.flip_type = static_cast<TileFlipType>(i);
//				out_tileset.flipToIndex.insert(pair<TileFlip, TileFlipIndex>(tile.flips[i], flip_index));
//			}
//			out_tileset.tiles.push_back(tile);
//			return true;
//		});
//	if(!success)
//	{
//		return false;
//	}
//	const size_t tile_count = out_tileset.tiles.size();
//	if(tile_count > kTilesMaxCount)
//	{
//		cout << "Too many tiles in the tileset (" << tile_count << " > " << kTilesMaxCount << ")" << endl;
//		return false;
//	}
//	return true;
//}

