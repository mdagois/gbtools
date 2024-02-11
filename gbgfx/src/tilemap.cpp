#include "log.h"
#include "tilemap.h"

namespace gbgfx {

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
		LOG_ERROR(
			"Tilemap is (" << tile_column_count << "x" << tile_row_count
			<< "), which is beyond the maximum of ("
			<< kTilemapColumnMaxCount << "x" << kTilemapRowMaxCount << ")");
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
	TilemapEntry entry;
	entry.tile_index = tile_index;
	entry.palette_index = palette;
	entry.bank = bank;
	entry.flip_horizontal = flip_horizontal;
	entry.flip_vertical = flip_vertical;
	entry.priority = priority;
	m_entries.push_back(entry);
}

const TilemapEntry& Tilemap::operator[](int32_t index) const
{
	return m_entries[index];
}

uint32_t Tilemap::size() const
{
	return static_cast<uint32_t>(m_entries.size());
}

uint32_t Tilemap::getRowCount() const
{
	return m_row_count;
}

uint32_t Tilemap::getColumnCount() const
{
	return m_column_count;
}

////////////////////////////////////////////////////////////////////////////////

}

