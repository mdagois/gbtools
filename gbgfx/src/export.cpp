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

