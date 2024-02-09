#pragma once

#include <vector>

#include "constants.h"
#include "palette.h"
#include "tileset.h"

////////////////////////////////////////////////////////////////////////////////

class PaletteSetData
{
public:
	PaletteSetData();
	virtual ~PaletteSetData();

	bool initialize(const PaletteSet& palette_set);

	const uint8_t* getData() const;
	uint32_t getDataSize() const;

private:
	struct PaletteData
	{
		ColorBGR555 colors[kColorsPerPalette];
	};

private:
	std::vector<PaletteData> m_data;
};

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

