#include "tileset.h"

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

static void flipRow(uint8_t* out, const uint8_t* in)
{
	out[0] = in[7];
	out[1] = in[6];
	out[2] = in[5];
	out[3] = in[4];
	out[4] = in[3];
	out[5] = in[2];
	out[6] = in[1];
	out[7] = in[0];
}

static void copyRow(uint8_t* out, const uint8_t* in)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	out[3] = in[3];
	out[4] = in[4];
	out[5] = in[5];
	out[6] = in[6];
	out[7] = in[7];
}

static void generateFlips(TileFlip* inout_tile_flips)
{
	const TileFlip& none = inout_tile_flips[kTileFlipType_None];

	{
		TileFlip& horizontal = inout_tile_flips[kTileFlipType_Horizontal];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			const uint32_t base_index = i * kTileSize;
			flipRow(
				horizontal.color_indices + base_index,
				none.color_indices + base_index);
		}
	}

	{
		TileFlip& vertical = inout_tile_flips[kTileFlipType_Vertical];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			const uint32_t in_index = i * kTileSize;
			const uint32_t out_index = ((kTileSize - 1) - i) * kTileSize;
			copyRow(
				vertical.color_indices + out_index,
				none.color_indices + in_index);
		}
	}

	{
		TileFlip& both = inout_tile_flips[kTileFlipType_Both];
		const TileFlip& horizontal = inout_tile_flips[kTileFlipType_Horizontal];
		for(uint32_t i = 0; i < kTileSize; ++i)
		{
			const uint32_t in_index = i * kTileSize;
			const uint32_t out_index = ((kTileSize - 1) - i) * kTileSize;
			copyRow(
				both.color_indices + out_index,
				horizontal.color_indices + in_index);
		}
	}
}

Tile::Tile()
{
}

Tile::~Tile()
{
}

void Tile::initialize(const TileFlip& tile_flip, uint32_t palette_index)
{
	m_flips[kTileFlipType_None] = tile_flip;
	generateFlips(m_flips);
	m_palette_index = palette_index;
}

////////////////////////////////////////////////////////////////////////////////
// Tileset
////////////////////////////////////////////////////////////////////////////////

Tileset::Tileset()
{
}

Tileset::~Tileset()
{
}

void Tileset::push(const Tile& tile)
{
	m_tiles.push_back(tile);
	//TODO Fill the map
}

uint32_t Tileset::size() const
{
	return static_cast<uint32_t>(m_tiles.size());
}

