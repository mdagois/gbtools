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

struct TileFlipIndex
{
	size_t index;
	TileFlipType flip_type;
};

class Tileset
{
public:
	Tileset();
	virtual ~Tileset();

	void push(const Tile& tile);
	uint32_t size() const;

private:
	struct FlipToIndexKey
	{
		TileFlip flip;
		uint32_t palette_index;

		bool operator<(const FlipToIndexKey& other) const;
	};

private:
	std::vector<Tile> m_tiles;
	std::map<FlipToIndexKey, TileFlipIndex> m_flip_to_index;
};

