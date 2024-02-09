#pragma once

#include <vector>

#include "constants.h"
#include "tileset.h"

////////////////////////////////////////////////////////////////////////////////

class TilesetData
{
public:
	TilesetData();
	virtual ~TilesetData();

	bool initialize(const Tileset& tileset);

	const uint8_t* getData() const;
	uint32_t getDataSize() const;

private:
	enum : uint32_t
	{
		kBytesPerTileDataRow = 2,
		kBytesPerTileData = kTileSize * kBytesPerTileDataRow,
	};

	struct TileData
	{
		uint8_t byte[kBytesPerTileData];
	};

private:
	std::vector<TileData> m_data;
};

