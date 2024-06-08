#pragma once

#include <cstdint>
#include <vector>

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Tilemap
////////////////////////////////////////////////////////////////////////////////

struct TilemapEntry
{
	uint8_t tile_index;
	uint8_t palette_index;
	uint8_t bank;
	bool flip_horizontal;
	bool flip_vertical;
	bool priority;
};

class Tilemap
{
public:
	Tilemap();
	virtual ~Tilemap();

	void add(
		uint8_t tile_index,
		uint8_t palette, uint8_t bank,
		bool flip_horizontal, bool flip_vertical,
		bool priority);

	const TilemapEntry& operator[](int32_t index) const;
	uint32_t size() const;

private:
	std::vector<TilemapEntry> m_entries;
};

////////////////////////////////////////////////////////////////////////////////

}

