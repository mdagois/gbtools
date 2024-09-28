#include <cassert>

#include "capabilities.h"
#include "palette.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Palette
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
	switch(CAPS.palette.format)
	{
		case kFormat_Color_BGR555:
			for(uint32_t p = 0; p < palette_set.size(); ++p)
			{
				const Palette& palette = palette_set[p];
				uint32_t c = 0;
				for(; c < palette.size(); ++c)
				{
					m_data_bgr555.push_back(convertColor(palette[c]));
				}
				for(; c < CAPS.palette.color_max_count; ++c)
				{
					m_data_bgr555.push_back(kBGR555_Invalid);
				}
			}
			break;
		case kFormat_Color_IDX2x4:
			for(uint32_t p = 0; p < palette_set.size(); ++p)
			{
				const Palette& palette = palette_set[p];
				ColorIndex index = 0;
				uint32_t shift = 0;
				for(uint32_t c = 0; c < palette.size(); ++c)
				{
					const ColorIndex color_index = convertColor(palette[c], CAPS.palette.fixed_colors, CAPS.palette.fixed_color_count);
					assert(color_index < 16);
					index |= color_index << shift;
					shift += 4;
				}
				m_data_2x4.push_back(index);
			}
			break;
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
	switch(CAPS.palette.format)
	{
		case kFormat_Color_BGR555:
			return reinterpret_cast<const uint8_t*>(m_data_bgr555.data());
		case kFormat_Color_IDX2x4:
			return reinterpret_cast<const uint8_t*>(m_data_2x4.data());
		default:
			assert(false);
			break;
	}
	return nullptr;
}

uint32_t PaletteSetData::getDataSize() const
{
	switch(CAPS.palette.format)
	{
		case kFormat_Color_BGR555:
			return static_cast<uint32_t>(m_data_bgr555.size() * sizeof(ColorBGR555));
		case kFormat_Color_IDX2x4:
			return static_cast<uint32_t>(m_data_2x4.size() * sizeof(ColorIndex));
		default:
			assert(false);
			break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

}

