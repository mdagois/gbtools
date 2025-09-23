#include <cassert>
#include <cstring>
#include <set>

#include "tileset.h"
#include "utils/log.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Tile flip
////////////////////////////////////////////////////////////////////////////////

TileFlip::TileFlip()
: width(0)
, height(0)
{
}

void initializeTileFlip(TileFlip& out_tile_flip, uint32_t width, uint32_t height)
{
	out_tile_flip.width = width;
	out_tile_flip.height = height;
	out_tile_flip.color_indices.resize(width * height);
}

bool operator==(const TileFlip& lhs, const TileFlip& rhs)
{
	return
		lhs.color_indices.size() == rhs.color_indices.size() &&
		memcmp(
			lhs.color_indices.data(), rhs.color_indices.data(),
			lhs.color_indices.size() * sizeof(lhs.color_indices[0])) == 0;
}

bool operator<(const TileFlip& lhs, const TileFlip& rhs)
{
	return
		lhs.color_indices.size() == rhs.color_indices.size() &&
		memcmp(
			lhs.color_indices.data(), rhs.color_indices.data(),
			lhs.color_indices.size() * sizeof(lhs.color_indices[0])) < 0;
}

static void flipRow(uint8_t* out, const uint8_t* in, uint32_t row_length)
{
	assert(out != in);
	for(uint32_t i = 0; i < row_length; ++i)
	{
		out[i] = in[row_length - (i + 1)];
	}
}

static void copyRow(uint8_t* out, const uint8_t* in, uint32_t row_length)
{
	assert(out != in);
	for(uint32_t i = 0; i < row_length; ++i)
	{
		out[i] = in[i];
	}
}

static bool isTileFlipValid(const TileFlip& tile_flip)
{
	return
		tile_flip.width > 0 && tile_flip.height > 0 &&
		tile_flip.color_indices.size() == tile_flip.width * tile_flip.height;
}

static void generateFlips(TileFlip* inout_tile_flips)
{
	const TileFlip& none = inout_tile_flips[kTileFlipType_None];
	const uint32_t width = none.width;
	const uint32_t height = none.height;
	assert(isTileFlipValid(none));

	{
		TileFlip& horizontal = inout_tile_flips[kTileFlipType_Horizontal];
		initializeTileFlip(horizontal, width, height);
		for(uint32_t i = 0; i < height; ++i)
		{
			const uint32_t base_index = i * width;
			flipRow(
				horizontal.color_indices.data() + base_index,
				none.color_indices.data() + base_index,
				width);
		}
	}

	{
		TileFlip& vertical = inout_tile_flips[kTileFlipType_Vertical];
		initializeTileFlip(vertical, width, height);
		for(uint32_t i = 0; i < height; ++i)
		{
			const uint32_t in_index = i * width;
			const uint32_t out_index = ((width - 1) - i) * width;
			copyRow(
				vertical.color_indices.data() + out_index,
				none.color_indices.data() + in_index,
				width);
		}
	}

	{
		TileFlip& both = inout_tile_flips[kTileFlipType_Both];
		initializeTileFlip(both, width, height);
		const TileFlip& horizontal = inout_tile_flips[kTileFlipType_Horizontal];
		for(uint32_t i = 0; i < height; ++i)
		{
			const uint32_t in_index = i * width;
			const uint32_t out_index = ((width - 1) - i) * width;
			copyRow(
				both.color_indices.data() + out_index,
				horizontal.color_indices.data() + in_index,
				width);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

Tile::Tile()
: m_palette_index(kInvalidIndex)
{
}

Tile::~Tile()
{
}

void Tile::setPaletteIndex(uint32_t palette_index)
{
	assert(m_palette_index == kInvalidIndex);
	m_palette_index = palette_index;
}

void Tile::initializeFlips(const TileFlip& none_tile_flip)
{
	assert(isTileFlipValid(none_tile_flip));
	m_flips[kTileFlipType_None] = none_tile_flip;
	generateFlips(m_flips);
}

const TileFlip& Tile::getTileFlip(TileFlipType type) const
{
	assert(type < kTileFlipType_Count);
	return m_flips[type];
}

uint32_t Tile::getPaletteIndex() const
{
	assert(m_palette_index != kInvalidIndex);
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

void Tileset::add(const Tile& tile)
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
	const size_t original_tile_count = m_tiles.size();

	struct CheckEntry
	{
		TileFlip flip;
		uint32_t palette_index;

		bool operator<(const CheckEntry& other) const
		{
			if(palette_index == other.palette_index)
			{
				return flip < other.flip;
			}
			return palette_index < other.palette_index;
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

	GFX_LOG_INFO(
		"Tileset shrinked from " << original_tile_count
		<< " tiles to " << m_tiles.size() << " tiles");
}

bool Tileset::findTileIndex(
	uint32_t& out_tile_index, TileFlipType& out_flip_type,
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
				out_flip_type = static_cast<TileFlipType>(flip_type);
				return true;
			}
			++flip_type;
		} while(compare_flips && flip_type != kTileFlipType_Count);
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////

}

