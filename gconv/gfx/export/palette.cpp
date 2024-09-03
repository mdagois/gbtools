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
	for(uint32_t p = 0; p < palette_set.size(); ++p)
	{
		const Palette& palette = palette_set[p];
		uint32_t c = 0;
		for(; c < palette.size(); ++c)
		{
			m_data.push_back(convertColor(palette[c]));
		}
		for(; c < CAPS.palette.color_max_count; ++c)
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

}

