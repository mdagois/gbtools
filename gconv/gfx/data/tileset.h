#pragma once

#include <cstdint>
#include <vector>

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Tile flip
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
	TileFlip();

	std::vector<uint8_t> color_indices;
	uint32_t width;
	uint32_t height;
};

void initializeTileFlip(TileFlip& out_tile_flip, uint32_t width, uint32_t height);
bool operator==(const TileFlip& lhs, const TileFlip& rhs);
bool operator<(const TileFlip& lhs, const TileFlip& rhs);

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

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

	void add(const Tile& tile);

	const Tile& operator[](int32_t index) const;
	uint32_t size() const;

	void removeDoubles(bool compare_flips);
	bool findTileIndex(
		uint32_t& out_tile_index, uint32_t& out_palette_index, TileFlipType& out_flip_type,
		const Tile& tile, bool compare_flips) const;

private:
	std::vector<Tile> m_tiles;
};

////////////////////////////////////////////////////////////////////////////////

}

