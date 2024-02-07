#include "tileset.h"

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

Tile::Tile()
{
}

Tile::~Tile()
{
}

void Tile::generateFlips()
{
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

uint32_t Tileset::size() const
{
	return m_tiles.size();
}

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

