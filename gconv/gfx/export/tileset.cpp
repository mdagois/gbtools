#include <cassert>

#include "capabilities.h"
#include "tileset.h"
#include "utils/log.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Tileset
////////////////////////////////////////////////////////////////////////////////

TilesetData::TilesetData()
: m_tile_count(0)
{
}

TilesetData::~TilesetData()
{
}

bool TilesetData::initialize(const Tileset& tileset)
{
	if( CAPS.tileset.color_index_format != kFormat_COL2222 &&
		CAPS.tileset.color_index_format != kFormat_COL2222_COL2222_INTERLEAVED)
	{
		GFX_LOG_ERROR("Unsupported tile format");
		return false;
	}
	for(uint32_t t = 0; t < tileset.size(); ++t)
	{
		const Tile& tile = tileset[t];
		const TileFlip& flip = tile.getTileFlip(kTileFlipType_None);
		if(flip.width != 8 || flip.height != 8)
		{
			GFX_LOG_ERROR("Unsupported flip size");
			return false;
		}
		if(CAPS.tileset.color_index_format == kFormat_COL2222)
		{
			for(uint32_t d = 0; d < flip.height; ++d)
			{
				const uint8_t* indices = flip.color_indices.data() + d * flip.width;
				m_data.push_back(
					((indices[0] << 7) & 0x80) | ((indices[1] << 6) & 0x40) | ((indices[2] << 5) & 0x20) | ((indices[3] << 4) & 0x10) |
					((indices[4] << 3) & 0x08) | ((indices[5] << 2) & 0x04) | ((indices[6] << 1) & 0x02) | ((indices[7] << 0) & 0x01));
				m_data.push_back(
					((indices[0] << 6) & 0x80) | ((indices[1] << 5) & 0x40) | ((indices[2] << 4) & 0x20) | ((indices[3] << 3) & 0x10) |
					((indices[4] << 2) & 0x08) | ((indices[5] << 1) & 0x04) | ((indices[6] << 0) & 0x02) | ((indices[7] >> 1) & 0x01));
			}
		}
		if(CAPS.tileset.color_index_format == kFormat_COL2222_COL2222_INTERLEAVED)
		{
			for(uint32_t d = 0; d < flip.height; ++d)
			{
				const uint8_t* indices = flip.color_indices.data() + d * flip.width;
				m_data.push_back(
					((indices[0] << 5) & 0x80) | ((indices[1] << 4) & 0x40) | ((indices[2] << 3) & 0x20) | ((indices[3] << 2) & 0x10) |
					((indices[4] << 1) & 0x08) | ((indices[5] << 0) & 0x04) | ((indices[6] >> 1) & 0x02) | ((indices[7] >> 2) & 0x01));
				m_data.push_back(
					((indices[0] << 4) & 0x80) | ((indices[1] << 3) & 0x40) | ((indices[2] << 2) & 0x20) | ((indices[3] << 0) & 0x10) |
					((indices[4] << 0) & 0x08) | ((indices[5] >> 1) & 0x04) | ((indices[6] >> 2) & 0x02) | ((indices[7] >> 3) & 0x01));
			}
		}
	}

	m_tile_count = tileset.size();
	return true;
}

uint32_t TilesetData::getTileCount() const
{
	return m_tile_count;
}

const uint8_t* TilesetData::getData() const
{
	return reinterpret_cast<const uint8_t*>(m_data.data());
}

uint32_t TilesetData::getDataSize() const
{
	return static_cast<uint32_t>(m_data.size() * sizeof(uint8_t));
}

////////////////////////////////////////////////////////////////////////////////

}

