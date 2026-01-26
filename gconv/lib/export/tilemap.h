#pragma once

#include <vector>

#include "data/tilemap.h"

namespace gfx {

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

