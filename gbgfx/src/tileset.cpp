#include <cassert>
#include <set>

#include "log.h"
#include "tileset.h"

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

bool operator==(const TileFlip& lhs, const TileFlip& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(TileFlip)) == 0;
}

bool operator<(const TileFlip& lhs, const TileFlip& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(TileFlip)) < 0;
}

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

const TileFlip& Tile::getTileFlip(TileFlipType type) const
{
	assert(type < kTileFlipType_Count);
	return m_flips[type];
}

uint32_t Tile::getPaletteIndex() const
{
	return m_palette_index;
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
}

const Tile& Tileset::operator[](int32_t index) const
{
	return m_tiles[index];
}

uint32_t Tileset::size() const
{
	return static_cast<uint32_t>(m_tiles.size());
}

void Tileset::removeDoubles(bool compare_flips)
{
	const uint32_t tile_count = m_tiles.size();

	struct CheckEntry
	{
		TileFlip flip;
		uint32_t palette_index;

		bool operator<(const CheckEntry& other) const
		{
			return
				palette_index < other.palette_index ||
				flip < other.flip;
		}
	};
	std::set<CheckEntry> check_set;

	auto tileIt = m_tiles.begin();
	while(tileIt != m_tiles.end())
	{
		const Tile& tile = *tileIt;
		CheckEntry entry;
		entry.palette_index = tile.getPaletteIndex();
		entry.flip = tile.getTileFlip(kTileFlipType_None);
		if(check_set.find(entry) != check_set.end())
		{
			tileIt = m_tiles.erase(tileIt);
			continue;
		}

		check_set.insert(entry);
		if(compare_flips)
		{
			entry.flip = tile.getTileFlip(kTileFlipType_Horizontal);
			check_set.insert(entry);
			entry.flip = tile.getTileFlip(kTileFlipType_Vertical);
			check_set.insert(entry);
			entry.flip = tile.getTileFlip(kTileFlipType_Both);
			check_set.insert(entry);
		}
		++tileIt;
	}

	LOG_INFO("Tileset shrinked from " << tile_count << " tiles to " << m_tiles.size() << " tiles");
}

bool Tileset::findTileIndex(
	uint32_t& out_tile_index, uint32_t& out_palette_index, TileFlipType& out_flip_type,
	const Tile& tile, bool compare_flips) const
{
	const uint32_t palette_index = tile.getPaletteIndex();
	const TileFlip& flip = tile.getTileFlip(kTileFlipType_None);

	for(uint32_t i = 0; i < m_tiles.size(); ++i)
	{
		const Tile& compare_tile = m_tiles[i];
		if(palette_index != compare_tile.getPaletteIndex())
		{
			continue;
		}

		uint32_t flip_type = kTileFlipType_None;
		do
		{
			if(flip == compare_tile.getTileFlip(static_cast<TileFlipType>(flip_type)))
			{
				out_tile_index = i;
				out_palette_index = palette_index;
				out_flip_type = static_cast<TileFlipType>(flip_type);
				return true;
			}
			++flip_type;
		} while(compare_flips && flip_type != kTileFlipType_Count);
	}

	return false;
}

