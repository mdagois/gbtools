#include "tilemap.h"
#include "utils/log.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////

Tilemap::Tilemap()
{
}

Tilemap::~Tilemap()
{
}

void Tilemap::add(
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

////////////////////////////////////////////////////////////////////////////////

}

