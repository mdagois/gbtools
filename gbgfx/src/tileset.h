#pragma once

#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

//TODO
typedef uint32_t Tile;

////////////////////////////////////////////////////////////////////////////////
// Tileset
////////////////////////////////////////////////////////////////////////////////

class Tileset
{
public:
	Tileset();
	virtual ~Tileset();

	void push(const Tile& tile);
	uint32_t size() const;

private:
};

//enum TileFlipType : uint32_t
//{
//	kTileFlipType_None,
//	kTileFlipType_Horizontal,
//	kTileFlipType_Vertical,
//	kTileFlipType_Both,
//	kTileFlipType_Count,
//};

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

//struct Tile
//{
//	TileFlip flips[kTileFlipType_Count];
//	uint32_t palette_index;
//};

//struct TileFlipIndex
//{
//	size_t index;
//	TileFlipType flip_type;
//};

//struct Tileset
//{
//	vector<Tile> tiles;
//	map<TileFlip, TileFlipIndex> flipToIndex;
//};

//static void generateTileFlips(Tile& out_tile)
//static bool findMatchingPaletteInSet(uint32_t& out_palette_index, const PaletteSet& palette_set, const Palette& palette)
//static bool extractTile(Tile& out_tile, const PaletteSet& palette_set, const ColorRGBA* pixels, uint32_t row_pitch)
//static bool extractTileset(Tileset& out_tileset, const PaletteSet& palette_set, const Image& image, bool optimize_tileset)

