#include <cassert>

#include "api.h"
#include "export.h"
#include "log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

PaletteSetData::PaletteSetData()
: m_palette_count(0)
{
}

PaletteSetData::~PaletteSetData()
{
}

bool PaletteSetData::initialize(const PaletteSet& palette_set)
{
	for(uint32_t p = 0; p < palette_set.size(); ++p)
	{
		const Palette& palette = palette_set[p];
		uint32_t c = 0;
		for(; c < palette.size(); ++c)
		{
			m_data.push_back(convertColor(palette[c]));
		}
		for(; c < getPaletteColorMaxCount(); ++c)
		{
			m_data.push_back(kBGR555_Invalid);
		}
	}
	m_palette_count = palette_set.size();
	return true;
}

uint32_t PaletteSetData::getPaletteCount() const
{
	return m_palette_count;
}

const uint8_t* PaletteSetData::getData() const
{
	return reinterpret_cast<const uint8_t*>(m_data.data());
}

uint32_t PaletteSetData::getDataSize() const
{
	return static_cast<uint32_t>(m_data.size() * sizeof(ColorBGR555));
}

////////////////////////////////////////////////////////////////////////////////

TilesetData::TilesetData()
: m_tile_count(0)
{
}

TilesetData::~TilesetData()
{
}

bool TilesetData::initialize(const Tileset& tileset)
{
	const bool is_sfc = getTargetHardware() == kHardwareSfc;
	for(uint32_t t = 0; t < tileset.size(); ++t)
	{
		const Tile& tile = tileset[t];
		const TileFlip& flip = tile.getTileFlip(kTileFlipType_None);
		for(uint32_t d = 0; d < kTileSize; ++d)
		{
			const uint8_t* indices = flip.color_indices + d * kTileSize;
			m_data.push_back(
				((indices[0] << 7) & 0x80) | ((indices[1] << 6) & 0x40) | ((indices[2] << 5) & 0x20) | ((indices[3] << 4) & 0x10) |
				((indices[4] << 3) & 0x08) | ((indices[5] << 2) & 0x04) | ((indices[6] << 1) & 0x02) | ((indices[7] << 0) & 0x01));
			m_data.push_back(
				((indices[0] << 6) & 0x80) | ((indices[1] << 5) & 0x40) | ((indices[2] << 4) & 0x20) | ((indices[3] << 3) & 0x10) |
				((indices[4] << 2) & 0x08) | ((indices[5] << 1) & 0x04) | ((indices[6] << 0) & 0x02) | ((indices[7] >> 1) & 0x01));
		}
		if(is_sfc)
		{
			for(uint32_t d = 0; d < kTileSize; ++d)
			{
				const uint8_t* indices = flip.color_indices + d * kTileSize;
				m_data.push_back(
					((indices[0] << 5) & 0x80) | ((indices[1] << 4) & 0x40) | ((indices[2] << 3) & 0x20) | ((indices[3] << 2) & 0x10) |
					((indices[4] << 1) & 0x08) | ((indices[5] << 0) & 0x04) | ((indices[6] >> 1) & 0x02) | ((indices[7] >> 2) & 0x01));
				m_data.push_back(
					((indices[0] << 4) & 0x80) | ((indices[1] << 3) & 0x40) | ((indices[2] << 2) & 0x20) | ((indices[3] << 0) & 0x10) |
					((indices[4] << 0) & 0x08) | ((indices[5] >> 1) & 0x04) | ((indices[6] >> 2) & 0x02) | ((indices[7] >> 3) & 0x01));
			}
		}
	}
	assert(getDataSize() % (is_sfc ? kBytesPerTileData_SFC : kBytesPerTileData_GB) == 0);
	m_tile_count = tileset.size();
	return true;
}

uint32_t TilesetData::getTileCount() const
{
	return m_tile_count;
}

const uint8_t* TilesetData::getData() const
{
	return reinterpret_cast<const uint8_t*>(m_data.data());
}

uint32_t TilesetData::getDataSize() const
{
	return static_cast<uint32_t>(m_data.size() * sizeof(uint8_t));
}

////////////////////////////////////////////////////////////////////////////////

TilemapData::TilemapData()
: m_row_count(0)
, m_column_count(0)
{
}

TilemapData::~TilemapData()
{
}

bool TilemapData::initialize(
	const Tilemap& tilemap,
	uint8_t palette_index_offset, uint8_t tile_index_offset)
{
	if(palette_index_offset >= getPaletteMaxCount())
	{
		GBGFX_LOG_ERROR(
			"The palette offset [" << palette_index_offset
			<< "] is over the palette max count [" << getPaletteMaxCount() << "]");
		return false;
	}
	const Hardware hardware = getTargetHardware();
	const bool extract_attributes = hardware == kHardwareSgb;

	uint8_t attribute = 0;
	uint32_t attribute_shift = 6;
	for(uint32_t i = 0; i < tilemap.size(); ++i)
	{
		const TilemapEntry& entry = tilemap[i];

		if(static_cast<uint32_t>(entry.palette_index) + palette_index_offset >= getPaletteMaxCount())
		{
			GBGFX_LOG_ERROR(
				"The palette index with offset [" << entry.palette_index + palette_index_offset
				<< "] is over the palette max count [" << getPaletteMaxCount() << "]");
			return false;
		}

		if(static_cast<uint32_t>(entry.tile_index) + tile_index_offset >= getTileMaxCount())
		{
			GBGFX_LOG_ERROR(
				"The tile index with offset [" << entry.tile_index + tile_index_offset
				<< "] is over the tile max count [" << getTileMaxCount() << "]");
			return false;
		}

		m_indices.push_back(entry.tile_index + tile_index_offset);
		m_parameters.push_back(
			((entry.palette_index + palette_index_offset) & 0x07) |
			((entry.bank & 0x01) << 3) |
			(entry.flip_horizontal ? 0x20 : 0x00) |
			(entry.flip_vertical ? 0x40 : 0x00) |
			(entry.priority ? 0x80 : 0x00));

		if(hardware == kHardwareSfc)
		{
			m_border_parameters.push_back(
				((entry.tile_index + tile_index_offset) & 0xFF) |
				(((entry.palette_index + palette_index_offset) & 0x03) << 10) |
				(entry.flip_horizontal ? 0x4000 : 0x0000) |
				(entry.flip_vertical ? 0x8000 : 0x0000));
		}

		if(extract_attributes)
		{
			attribute |= ((entry.palette_index & 0x3) << attribute_shift);
			if(attribute_shift == 0)
			{
				m_attributes.push_back(attribute);
				attribute = 0;
				attribute_shift = 6;
			}
			else
			{
				attribute_shift -= 2;
			}
		}
	}
	if(extract_attributes)
	{
		if(attribute_shift != 0)
		{
			m_attributes.push_back(attribute);
		}
	}

	m_row_count = tilemap.getRowCount();
	m_column_count = tilemap.getColumnCount();
	return true;
}

uint32_t TilemapData::getRowCount() const
{
	return m_row_count;
}

uint32_t TilemapData::getColumnCount() const
{
	return m_column_count;
}

const uint8_t* TilemapData::getIndexData() const
{
	return reinterpret_cast<const uint8_t*>(m_indices.data());
}

const uint8_t* TilemapData::getParameterData() const
{
	if(getTargetHardware() == kHardwareCgb)
	{
		return reinterpret_cast<const uint8_t*>(m_parameters.data());
	}
	if(getTargetHardware() == kHardwareSfc)
	{
		return reinterpret_cast<const uint8_t*>(m_border_parameters.data());
	}
	return nullptr;
}

const uint8_t* TilemapData::getAttributeData() const
{
	return reinterpret_cast<const uint8_t*>(m_attributes.data());
}

uint32_t TilemapData::getIndexDataSize() const
{
	return static_cast<uint32_t>(m_indices.size() * sizeof(uint8_t));
}

uint32_t TilemapData::getParameterDataSize() const
{
	if(getTargetHardware() == kHardwareCgb)
	{
		return static_cast<uint32_t>(m_parameters.size() * sizeof(uint8_t));
	}
	if(getTargetHardware() == kHardwareSfc)
	{
		return static_cast<uint32_t>(m_border_parameters.size() * sizeof(uint16_t));
	}
	return 0;
}

uint32_t TilemapData::getAttributeDataSize() const
{
	return static_cast<uint32_t>(m_attributes.size() * sizeof(uint8_t));
}

////////////////////////////////////////////////////////////////////////////////

}

