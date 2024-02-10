#include "tilemap.h"

////////////////////////////////////////////////////////////////////////////////

Tilemap::Tilemap()
: m_row_count(0)
, m_column_count(0)
{
}

Tilemap::~Tilemap()
{
}

bool Tilemap::initialize(uint8_t tile_row_count, uint8_t tile_column_count)
{
	if(tile_row_count > kTilemapRowMaxCount || tile_column_count > kTilemapColumnMaxCount)
	{
		return false;
	}
	m_row_count = tile_row_count;
	m_column_count = tile_column_count;
	return true;
}

void Tilemap::push(
	uint8_t tile_index,
	uint8_t palette, uint8_t bank,
	bool flip_horizontal, bool flip_vertical,
	bool priority)
{
	m_tile_indices.push_back(tile_index);

	Parameter parameter;
	parameter.palette_index = palette;
	parameter.bank = bank;
	parameter.flip_horizontal = flip_horizontal;
	parameter.flip_vertical = flip_vertical;
	parameter.priority = priority;
	m_tile_parameters.push_back(parameter);
}

