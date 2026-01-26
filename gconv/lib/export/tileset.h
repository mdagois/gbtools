#pragma once

#include <vector>

#include "data/tileset.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Tileset
////////////////////////////////////////////////////////////////////////////////

class TilesetData
{
public:
	TilesetData();
	virtual ~TilesetData();

	bool initialize(const Tileset& tileset);

	uint32_t getTileCount() const;
	const uint8_t* getData() const;
	uint32_t getDataSize() const;

private:
	std::vector<uint8_t> m_data;
	uint32_t m_tile_count;
};

////////////////////////////////////////////////////////////////////////////////

}

