#pragma once

#include <vector>

#include "data/palette.h"
#include "data/tilemap.h"
#include "data/tileset.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

typedef uint16_t ColorBGR555;

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
	std::vector<ColorBGR555> m_data;
	uint32_t m_palette_count;
};

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

#if 0
////////////////////////////////////////////////////////////////////////////////
// Tilemap
////////////////////////////////////////////////////////////////////////////////

class TilemapData
{
public:
	TilemapData();
	virtual ~TilemapData();

	bool initialize(
		const Tilemap& tilemap,
		uint8_t palette_index_offset, uint8_t tile_index_offset,
		bool use_8800_addressing_mode);

	uint32_t getRowCount() const;
	uint32_t getColumnCount() const;
	const uint8_t* getIndexData() const;
	const uint8_t* getParameterData() const;
	const uint8_t* getAttributeData() const;
	uint32_t getIndexDataSize() const;
	uint32_t getParameterDataSize() const;
	uint32_t getAttributeDataSize() const;

private:
	std::vector<uint8_t> m_indices;
	std::vector<uint8_t> m_parameters_8;
	std::vector<uint16_t> m_parameters_16;
	std::vector<uint8_t> m_attributes;
	uint32_t m_row_count;
	uint32_t m_column_count;
};

////////////////////////////////////////////////////////////////////////////////
#endif

}

