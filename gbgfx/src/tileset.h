#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "constants.h"

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

enum TileFlipType : uint32_t
{
	kTileFlipType_None,
	kTileFlipType_Horizontal,
	kTileFlipType_Vertical,
	kTileFlipType_Both,
	kTileFlipType_Count,
};

struct TileFlip
{
	uint8_t color_indices[kPixelsPerTile];
};

bool operator==(const TileFlip& lhs, const TileFlip& rhs);
bool operator<(const TileFlip& lhs, const TileFlip& rhs);

class Tile
{
public:
	Tile();
	virtual ~Tile();

	void initialize(const TileFlip& tile_flip, uint32_t palette_index);
	const TileFlip& getTileFlip(TileFlipType type) const;
	uint32_t getPaletteIndex() const;

private:
	TileFlip m_flips[kTileFlipType_Count];
	uint32_t m_palette_index;
};

////////////////////////////////////////////////////////////////////////////////
// Tileset
////////////////////////////////////////////////////////////////////////////////

class Tileset
{
public:
	Tileset();
	virtual ~Tileset();

	void push(const Tile& tile);
	const Tile& operator[](int32_t index) const;
	uint32_t size() const;
	void removeDoubles(bool compare_flips);

private:
	std::vector<Tile> m_tiles;
};

