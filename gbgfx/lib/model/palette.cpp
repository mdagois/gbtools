#include <algorithm>
#include <cassert>
#include <set>

#include "log.h"
#include "palette.h"

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

static bool mergePalettes(Palette& out_palette, const Palette lhs, const Palette rhs, uint32_t color_max_count)
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

	if(colors.size() > color_max_count)
	{
		return false;
	}

	out_palette.clear();
	for(ColorRGBA color : colors)
	{
		out_palette.add(color);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

Palette::Palette(bool insert_transparent_color)
{
	if(insert_transparent_color)
	{
		add(kRGBA_Magenta);
	}
}

Palette::~Palette()
{
}

void Palette::add(ColorRGBA color)
{
	m_colors.push_back(color);
	sortColorsRGBA(m_colors.data(), static_cast<uint32_t>(m_colors.size()));
}

void Palette::clear()
{
	m_colors.clear();
}

uint32_t Palette::size() const
{
	return static_cast<uint32_t>(m_colors.size());
}

const ColorRGBA Palette::operator[](int32_t index) const
{
	assert(index >= 0 && index < static_cast<int32_t>(m_colors.size()));
	return m_colors[index];
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

bool Palette::findColorIndex(uint8_t& out_color_index, ColorRGBA color) const
{
	for(uint32_t i = 0; i < m_colors.size(); ++i)
	{
		if(color == m_colors[i])
		{
			out_color_index = static_cast<uint8_t>(i);
			return true;
		}
	}
	return false;
}

void Palette::makeFirstColor(ColorRGBA color)
{
	for(uint32_t i = 0; i < m_colors.size(); ++i)
	{
		if(color == m_colors[i])
		{
			m_colors[i].a = kHighPriorityColorAlpha;
			sortColorsRGBA(m_colors.data(), static_cast<uint32_t>(m_colors.size()));
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

void PaletteSet::add(const Palette& palette)
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

bool PaletteSet::optimize(uint32_t palette_color_max_count, bool share_first_color)
{
	const uint32_t palette_count = static_cast<uint32_t>(m_palettes.size());

	if(share_first_color)
	{
		std::vector<const Palette*> four_color_palettes;
		for(const Palette& palette : m_palettes)
		{
			if(palette.size() == palette_color_max_count)
			{
				four_color_palettes.push_back(&palette);
			}
		}
		if(four_color_palettes.empty())
		{
			for(Palette& palette : m_palettes)
			{
				palette.add(kRGBA_Magenta);
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
				if(palette.size() == palette_color_max_count)
				{
					palette.makeFirstColor(candidate_color);
				}
				else
				{
					palette.add(candidate_color);
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
				if(mergePalettes(m_palettes[i], m_palettes[i], palette, palette_color_max_count))
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

bool PaletteSet::findCompatiblePaletteIndex(uint32_t& out_palette_index, const Palette& palette) const
{
	for(uint32_t i = 0; i < m_palettes.size(); ++i)
	{
		if(m_palettes[i].contains(palette))
		{
			out_palette_index = i;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

}

