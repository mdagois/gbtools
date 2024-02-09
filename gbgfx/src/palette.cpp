#include <algorithm>
#include <cassert>
#include <set>
#include "palette.h"

////////////////////////////////////////////////////////////////////////////////

static bool hasColor(const Palette& palette, ColorRGBA color)
{
	for(uint32_t i = 0; i < palette.size(); ++i)
	{
		if(color == palette[i])
		{
			return true;
		}
	}
	return false;
}

static bool operator==(const Palette& lhs, const Palette& rhs)
{
    if(lhs.size() != rhs.size())
	{
		return false;
	}

	for(uint32_t i = 0; i < lhs.size(); ++i)
	{
		if(lhs[i] != rhs[i])
		{
			return false;
		}
	}

	return true;
}

static bool mergePalettes(Palette& out_palette, const Palette lhs, const Palette rhs)
{
	std::set<ColorRGBA> colors;
	for(uint32_t i = 0; i < lhs.size(); ++i)
	{
		colors.insert(lhs[i]);
	}
	for(uint32_t i = 0; i < rhs.size(); ++i)
	{
		colors.insert(rhs[i]);
	}

	if(colors.size() > kColorsPerPalette)
	{
		return false;
	}

	out_palette.clear();
	for(ColorRGBA color : colors)
	{
		out_palette.push(color);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

Palette::Palette()
: m_color_count(0)
{
	for(int32_t i = 0; i < kColorsPerPalette; ++i)
	{
		m_colors[i] = kRGBA_Invalid;
	}
}

Palette::~Palette()
{
}

void Palette::push(ColorRGBA color)
{
	assert(m_color_count < kColorsPerPalette);
	m_colors[m_color_count] = color;
	++m_color_count;
	sortColors(m_colors, m_color_count);
}

void Palette::clear()
{
	m_color_count = 0;
}

bool Palette::contains(const Palette& sub_palette) const
{
	for(uint32_t i = 0; i < sub_palette.size(); ++i)
	{
		if(!hasColor(*this, sub_palette[i]))
		{
			return false;
		}
	}
	return true;
}

uint32_t Palette::size() const
{
	return m_color_count;
}

const ColorRGBA Palette::operator[](int32_t index) const
{
	assert(index >= 0 && index < static_cast<int32_t>(m_color_count));
	return m_colors[index];
}

uint8_t Palette::findColorIndex(ColorRGBA color) const
{
	for(uint32_t i = 0; i < m_color_count; ++i)
	{
		if(color == m_colors[i])
		{
			return static_cast<uint8_t>(i);
		}
	}
	return kInvalidColorIndex;
}

////////////////////////////////////////////////////////////////////////////////
// Palette set
////////////////////////////////////////////////////////////////////////////////

PaletteSet::PaletteSet()
{
}

PaletteSet::~PaletteSet()
{
}

void PaletteSet::push(const Palette& palette)
{
	for(uint32_t i = 0; i < m_palettes.size(); ++i)
	{
		if(m_palettes[i].contains(palette))
		{
			return;
		}
	}
	m_palettes.push_back(palette);
}

const Palette& PaletteSet::operator[](int32_t index) const
{
	return m_palettes[index];
}

uint32_t PaletteSet::size() const
{
	return static_cast<uint32_t>(m_palettes.size());
}

void PaletteSet::optimize()
{
	if(size() < 2)
	{
		return;
	}

	std::vector<Palette> palettes = m_palettes;
	std::sort(
		palettes.begin(), palettes.end(),
		[](const Palette& lhs, const Palette& rhs)
		{
			return lhs.size() > rhs.size();
		});

	m_palettes.clear();
	m_palettes.push_back(palettes.back());
	palettes.pop_back();

	while(!palettes.empty())
	{
		const Palette& palette = palettes.back();
		bool merged = false;
		for(uint32_t i = 0; i < m_palettes.size(); ++i)
		{
			if(mergePalettes(m_palettes[i], m_palettes[i], palette))
			{
				merged = true;
				break;
			}
		}
		if(!merged)
		{
			m_palettes.push_back(palette);
		}
		palettes.pop_back();
	}
}

uint32_t PaletteSet::findCompatiblePaletteIndex(const Palette& palette) const
{
	for(uint32_t i = 0; i < m_palettes.size(); ++i)
	{
		if(m_palettes[i].contains(palette))
		{
			return i;
		}
	}
	return kInvalidPaletteIndex;
}

