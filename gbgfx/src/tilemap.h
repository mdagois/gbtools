#pragma once

#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTilemapRowMaxCount = 32,
	kTilemapColumnMaxCount = 32,
};

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

	bool initialize(uint8_t tile_row_count, uint8_t tile_column_count);
	void push(
		uint8_t tile_index,
		uint8_t palette, uint8_t bank,
		bool flip_horizontal, bool flip_vertical,
		bool priority);
	const TilemapEntry& operator[](int32_t index) const;
	uint32_t size() const;

private:
	std::vector<TilemapEntry> m_entries;
	uint32_t m_row_count;
	uint32_t m_column_count;
};

