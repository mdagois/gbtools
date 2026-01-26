#include <cassert>

#include "capabilities.h"
#include "tilemap.h"
#include "utils/log.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Tilemap
////////////////////////////////////////////////////////////////////////////////

TilemapData::TilemapData()
{
}

TilemapData::~TilemapData()
{
}

bool TilemapData::initialize(
	const Tilemap& tilemap,
	uint8_t palette_index_offset, uint8_t tile_index_offset,
	bool use_8800_addressing_mode)
{
	if(palette_index_offset >= CAPS.palette.max_count)
	{
		GFX_LOG_ERROR(
			"The palette offset [" << palette_index_offset
			<< "] is over the palette max count [" << CAPS.palette.max_count << "]");
		return false;
	}
	if(tile_index_offset >= CAPS.tileset.tile_max_count)
	{
		GFX_LOG_ERROR(
			"The tile offset [" << tile_index_offset
			<< "] is over the tile max count [" << CAPS.tileset.tile_max_count << "]");
		return false;
	}

	uint8_t attribute = 0;
	uint32_t attribute_shift = 6;
	for(uint32_t i = 0; i < tilemap.size(); ++i)
	{
		TilemapEntry entry = tilemap[i];

		if(static_cast<uint32_t>(entry.palette_index) + palette_index_offset >= CAPS.palette.max_count)
		{
			GFX_LOG_ERROR(
				"The palette index with offset [" << entry.palette_index + palette_index_offset
				<< "] is over the palette max count [" << CAPS.palette.max_count << "]");
			return false;
		}

		if(tile_index_offset > 0)
		{
			uint32_t global_tile_index = static_cast<uint32_t>(entry.tile_index) + (entry.bank * CAPS.tileset.tiles_per_bank);
			global_tile_index += tile_index_offset;
			if(global_tile_index >= CAPS.tileset.tile_max_count)
			{
				GFX_LOG_ERROR(
					"The tile index with offset [" << entry.tile_index + tile_index_offset
					<< "] is over the tile max count [" << CAPS.tileset.tile_max_count << "]");
				return false;
			}
			entry.tile_index = global_tile_index % CAPS.tileset.tiles_per_bank;
			entry.bank = global_tile_index / CAPS.tileset.tiles_per_bank;
			if(entry.bank >= CAPS.tileset.bank_max_count)
			{
				GFX_LOG_ERROR(
					"The bank (after applying the tile offset) [" << entry.bank
					<< "] is over the bank max count [" << CAPS.tileset.bank_max_count << "]");
				return false;
			}
		}

		if(use_8800_addressing_mode)
		{
			entry.tile_index += 128;
		}

		const uint8_t palette_index = entry.palette_index + palette_index_offset + CAPS.palette.base_index;

		if(CAPS.tilemap.tile_index_format == kFormat_IDX8)
		{
			m_indices.push_back(entry.tile_index + tile_index_offset);
		}

		switch(CAPS.tilemap.tile_parameter_format)
		{
			case kFormat_PAL3_BNK1_X1_FLP2_PRI1:
				m_parameters_8.push_back(
					(palette_index & 0x07) |
					((entry.bank & 0x01) << 3) |
					(entry.flip_horizontal ? 0x20 : 0x00) |
					(entry.flip_vertical ? 0x40 : 0x00) |
					(entry.priority ? 0x80 : 0x00));
				break;
			case kFormat_IDX8_X2_PAL3_X1_FLP2:
				m_parameters_16.push_back(
					((entry.tile_index + tile_index_offset) & 0xFF) |
					((palette_index & 0x03) << 10) |
					(entry.flip_horizontal ? 0x4000 : 0x0000) |
					(entry.flip_vertical ? 0x8000 : 0x0000));
				break;
			case kFormat_PAL2222:
				attribute |= ((palette_index & 0x3) << attribute_shift);
				if(attribute_shift == 0)
				{
					m_parameters_8.push_back(attribute);
					attribute = 0;
					attribute_shift = 6;
				}
				else
				{
					attribute_shift -= 2;
				}
				break;
			default:
				break;
		}
	}
	if(	CAPS.tilemap.tile_parameter_format == kFormat_PAL2222 &&
		attribute_shift != 0)
	{
		m_parameters_8.push_back(attribute);
	}

	return true;
}

const uint8_t* TilemapData::getIndexData() const
{
	return reinterpret_cast<const uint8_t*>(m_indices.data());
}

const uint8_t* TilemapData::getParameterData() const
{
	switch(CAPS.tilemap.tile_parameter_format)
	{
		case kFormat_PAL3_BNK1_X1_FLP2_PRI1:
		case kFormat_PAL2222:
			return reinterpret_cast<const uint8_t*>(m_parameters_8.data());
		case kFormat_IDX8_X2_PAL3_X1_FLP2:
			return reinterpret_cast<const uint8_t*>(m_parameters_16.data());
		default:
			break;
	}
	return nullptr;
}

uint32_t TilemapData::getIndexDataSize() const
{
	return static_cast<uint32_t>(m_indices.size() * sizeof(uint8_t));
}

uint32_t TilemapData::getParameterDataSize() const
{
	switch(CAPS.tilemap.tile_parameter_format)
	{
		case kFormat_PAL3_BNK1_X1_FLP2_PRI1:
		case kFormat_PAL2222:
			return static_cast<uint32_t>(m_parameters_8.size() * sizeof(uint8_t));
		case kFormat_IDX8_X2_PAL3_X1_FLP2:
			return static_cast<uint32_t>(m_parameters_16.size() * sizeof(uint16_t));
		default:
			break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

}

