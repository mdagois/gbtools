#pragma once

#include <vector>

#include "tileset.h"

////////////////////////////////////////////////////////////////////////////////

struct TileData
{
	uint8_t rows[kTileSize];
};

class TilesetData
{
public:
	TilesetData();
	virtual ~TilesetData();

	bool initialize(const Tileset& tileset);

	const uint8_t* getData() const;
	uint32_t getDataSize() const;

private:
	std::vector<TileData> m_data;
};

