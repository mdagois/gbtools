#include <cassert>

#include "capabilities.h"
#include "palette.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// BGR555
////////////////////////////////////////////////////////////////////////////////

enum : ColorBGR555
{
	kBGR555_Invalid = 0x8000U,
};

static ColorBGR555 convertColor(ColorRGBA rgba)
{
	const uint8_t red = rgba.r / 8;
	const uint8_t green = rgba.g / 8;
	const uint8_t blue = rgba.b / 8;
	assert(red < 32 && green < 32 && blue < 32);
	return (blue << 10) | (green << 5) | red;
}

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
		for(; c < FEATURES.palette.color_max_count; ++c)
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

