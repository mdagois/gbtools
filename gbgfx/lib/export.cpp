#include <cassert>

#include "export.h"

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
		for(; c < Palette::getColorMaxCount(); ++c)
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
	for(uint32_t t = 0; t < tileset.size(); ++t)
	{
		const Tile& tile = tileset[t];
		const TileFlip& flip = tile.getTileFlip(kTileFlipType_None);
		TileData data;
		for(uint32_t d = 0; d < kTileSize; ++d)
		{
			const uint8_t* indices = flip.color_indices + d * kTileSize;
			uint8_t* bytes = data.byte + d * kBytesPerTileDataRow;
			bytes[0] =
				((indices[0] << 7) & 0x80) | ((indices[1] << 6) & 0x40) | ((indices[2] << 5) & 0x20) | ((indices[3] << 4) & 0x10) |
				((indices[4] << 3) & 0x08) | ((indices[5] << 2) & 0x04) | ((indices[6] << 1) & 0x02) | ((indices[7] << 0) & 0x01);
			bytes[1] =
				((indices[0] << 6) & 0x80) | ((indices[1] << 5) & 0x40) | ((indices[2] << 4) & 0x20) | ((indices[3] << 3) & 0x10) |
				((indices[4] << 2) & 0x08) | ((indices[5] << 1) & 0x04) | ((indices[6] << 0) & 0x02) | ((indices[7] >> 1) & 0x01);
		}
		m_data.push_back(data);
	}
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
	return static_cast<uint32_t>(m_data.size() * sizeof(TileData));
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
	uint8_t attribute = 0;
	uint32_t attribute_shift = 6;
	for(uint32_t i = 0; i < tilemap.size(); ++i)
	{
		const TilemapEntry& entry = tilemap[i];
		m_indices.push_back(entry.tile_index + tile_index_offset);
		m_parameters.push_back(
			((entry.palette_index + palette_index_offset) & 0x07) |
			((entry.bank & 0x01) << 3) |
			(entry.flip_horizontal ? 0x20 : 0x00) |
			(entry.flip_vertical ? 0x40 : 0x00) |
			(entry.priority ? 0x80 : 0x00));

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
	if(attribute_shift != 0)
	{
		m_attributes.push_back(attribute);
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
	return reinterpret_cast<const uint8_t*>(m_parameters.data());
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
	return static_cast<uint32_t>(m_parameters.size() * sizeof(uint8_t));
}

uint32_t TilemapData::getAttributeDataSize() const
{
	return static_cast<uint32_t>(m_attributes.size() * sizeof(uint8_t));
}

////////////////////////////////////////////////////////////////////////////////

}

