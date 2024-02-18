#include <algorithm>
#include <cassert>
#include <set>

#include "log.h"
#include "palette.h"
#include "profile.h"

namespace gbgfx {

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

	if(colors.size() > getPaletteColorMaxCount())
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
	for(int32_t i = 0; i < kColorsPerPalette_Max; ++i)
	{
		m_colors[i] = kRGBA_Invalid;
	}
	if(getUseTransparentColor())
	{
		push(kRGBA_Magenta);
	}
}

Palette::~Palette()
{
}

void Palette::push(ColorRGBA color)
{
	assert(m_color_count < kColorsPerPalette_Max);
	m_colors[m_color_count] = color;
	++m_color_count;
	sortColorsRGBA(m_colors, m_color_count);
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

void Palette::makeFirstColor(ColorRGBA color)
{
	for(uint32_t i = 0; i < m_color_count; ++i)
	{
		if(color == m_colors[i])
		{
			m_colors[i].a = kSharedColorAlpha;
			return;
		}
	}
	assert(false);
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
		if(palette.contains(m_palettes[i]))
		{
			m_palettes[i] = palette;
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

bool PaletteSet::optimize()
{
	const uint32_t palette_count = static_cast<uint32_t>(m_palettes.size());

	if(getTargetHardware() == kHardwareSgb)
	{
		std::vector<const Palette*> four_color_palettes;
		for(const Palette& palette : m_palettes)
		{
			if(palette.size() == getPaletteColorMaxCount())
			{
				four_color_palettes.push_back(&palette);
			}
		}
		if(four_color_palettes.empty())
		{
			for(Palette& palette : m_palettes)
			{
				palette.push(kRGBA_Magenta);
			}
		}
		else
		{
			std::vector<ColorRGBA> first_color_candidates;
			for(const Palette* palette : four_color_palettes)
			{
				for(uint32_t c = 0; c < palette->size(); ++c)
				{
					first_color_candidates.push_back((*palette)[c]);
				}
			}
			sortColorsRGBA(first_color_candidates.data(), static_cast<uint32_t>(first_color_candidates.size()));

			uint32_t count = 0;
			ColorRGBA candidate_color;
			bool first_color_found = false;
			for(ColorRGBA color : first_color_candidates)
			{
				if(count == 0 || color != candidate_color)
				{
					count = 1;
					candidate_color = color;
				}
				else
				{
					++count;
				}
				if(count == static_cast<uint32_t>(four_color_palettes.size()))
				{
					first_color_found = true;
					break;
				}
			}

			if(!first_color_found)
			{
				GBGFX_LOG_ERROR("Cannot find a shared first color");
				return false;
			}

			for(Palette& palette : m_palettes)
			{
				if(palette.size() == getPaletteColorMaxCount())
				{
					palette.makeFirstColor(candidate_color);
				}
				else
				{
					palette.push(candidate_color);
				}
			}
		}
	}

	if(palette_count >= 2)
	{
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

	GBGFX_LOG_INFO("Palette optimization from " << palette_count << " to " << m_palettes.size());
	return true;
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

////////////////////////////////////////////////////////////////////////////////

}

