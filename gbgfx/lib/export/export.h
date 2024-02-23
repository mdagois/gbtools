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

	const uint8_t* getIndexData() const;
	const uint8_t* getParameterData() const;
	uint32_t getIndexDataSize() const;
	uint32_t getParameterDataSize() const;

private:
	std::vector<uint8_t> m_indices;
	std::vector<uint8_t> m_parameters_8;
	std::vector<uint16_t> m_parameters_16;
};

////////////////////////////////////////////////////////////////////////////////

}

