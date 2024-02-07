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
	uint32_t m_color_indices[kPixelsPerTile];
};

class Tile
{
public:
	Tile();
	virtual ~Tile();

private:
	void generateFlips();

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
	std::vector<Tile> m_tiles;
	std::map<TileFlip, TileFlipIndex> m_flip_to_index;
};

