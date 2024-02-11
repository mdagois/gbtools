#pragma once

#include <vector>

#include "constants.h"
#include "palette.h"
#include "tilemap.h"
#include "tileset.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

class PaletteSetData
{
public:
	PaletteSetData();
	virtual ~PaletteSetData();

	bool initialize(const PaletteSet& palette_set);

	uint32_t getPaletteCount() const;
	const uint8_t* getData() const;
	uint32_t getDataSize() const;

private:
	struct PaletteData
	{
		ColorBGR555 colors[kColorsPerPalette];
	};

private:
	std::vector<PaletteData> m_data;
	uint32_t m_palette_count;
};

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
	uint32_t m_tile_count;
};

////////////////////////////////////////////////////////////////////////////////

class TilemapData
{
public:
	TilemapData();
	virtual ~TilemapData();

	bool initialize(const Tilemap& tilemap);

	uint32_t getRowCount() const;
	uint32_t getColumnCount() const;
	const uint8_t* getIndexData() const;
	const uint8_t* getParameterData() const;
	uint32_t getDataSize() const;

private:
	std::vector<uint8_t> m_indices;
	std::vector<uint8_t> m_parameters;
	uint32_t m_row_count;
	uint32_t m_column_count;
};

////////////////////////////////////////////////////////////////////////////////

}

