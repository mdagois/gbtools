#include "export.h"

////////////////////////////////////////////////////////////////////////////////

TilesetData::TilesetData()
{
}

TilesetData::~TilesetData()
{
}

bool TilesetData::initialize(const Tileset& tileset)
{
	for(uint32_t t = 0; t < tileset.size(); ++t)
	{
		const Tile& tile = tileset[t];
		const TileFlip& flip = tile.getTileFlip(kTileFlipType_None);
		TileData data;
		for(uint32_t d = 0; d < kTileSize; ++d)
		{
			const uint8_t* indices = flip.color_indices + d * kTileSize;
			uint8_t* bytes = data.byte + d * kBytesPerTileDataRow;
			bytes[0] =
				((indices[0] << 7) & 0x80) | ((indices[1] << 6) & 0x40) | ((indices[2] << 5) & 0x20) | ((indices[3] << 4) & 0x10) |
				((indices[4] << 3) & 0x08) | ((indices[5] << 2) & 0x04) | ((indices[6] << 1) & 0x02) | ((indices[7] << 0) & 0x01);
			bytes[1] =
				((indices[0] << 6) & 0x80) | ((indices[1] << 5) & 0x40) | ((indices[2] << 4) & 0x20) | ((indices[3] << 3) & 0x10) |
				((indices[4] << 2) & 0x08) | ((indices[5] << 1) & 0x04) | ((indices[6] << 0) & 0x02) | ((indices[7] >> 1) & 0x01);
		}
		m_data.push_back(data);
	}
	return true;
}

const uint8_t* TilesetData::getData() const
{
	return reinterpret_cast<const uint8_t*>(m_data.data());
}

uint32_t TilesetData::getDataSize() const
{
	return static_cast<uint32_t>(m_data.size() * sizeof(TileData));
}

