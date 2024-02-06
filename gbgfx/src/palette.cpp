#include <algorithm>
#include <cassert>
#include "palette.h"

////////////////////////////////////////////////////////////////////////////////

static bool operator==(const Palette& lhs, const Palette& rhs)
{
    if(lhs.getColorCount() != rhs.getColorCount())
	{
		return false;
	}

	for(uint32_t i = 0; i < lhs.getColorCount(); ++i)
	{
		if(lhs[i] != rhs[i])
		{
			return false;
		}
	}

	return true;
}

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

uint32_t Palette::getColorCount() const
{
	return m_color_count;
}

const ColorRGBA Palette::operator[](int32_t index) const
{
	assert(index >= 0 && index < static_cast<int32_t>(m_color_count));
	return m_colors[index];
}

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
		if(m_palettes[i] == palette)
		{
			return;
		}
	}
	m_palettes.push_back(palette);
}

uint32_t PaletteSet::getPaletteCount() const
{
	return static_cast<uint32_t>(m_palettes.size());
}

const Palette& PaletteSet::operator[](int32_t index) const
{
	return m_palettes[index];
}

void PaletteSet::optimize()
{
	std::sort(
		m_palettes.begin(), m_palettes.end(),
		[](const Palette& lhs, const Palette& rhs)
		{
			return lhs.getColorCount() > rhs.getColorCount();
		});
}

//bool mergePalettes(Palette& out_palette, const Palette lhs, const Palette rhs)
//{
//	set<ColorRGBA> colors;
//	for(uint32_t i = 0; i < kColorsPerPalette; ++i)
//	{
//		if(lhs.colors[i] == kBGR555_Invalid)
//		{
//			break;
//		}
//		colors.insert(lhs.colors[i]);
//	}
//	for(uint32_t i = 0; i < kColorsPerPalette; ++i)
//	{
//		if(rhs.colors[i] == kBGR555_Invalid)
//		{
//			break;
//		}
//		colors.insert(rhs.colors[i]);
//	}
//
//	if(colors.size() > kColorsPerPalette)
//	{
//		return false;
//	}
//
//	uint32_t c = 0;
//	for(ColorRGBA color : colors)
//	{
//		out_palette.colors[c] = color;
//		++c;
//	}
//	sortColors(out_palette.colors, c);
//	return true;
//}

//static bool mergePaletteIntoSet(PaletteSet& out_palette_set, const Palette palette)
//{
//	for(uint32_t p = 0; p < kPaletteMaxCount; ++p)
//	{
//		Palette& set_palette = out_palette_set.palettes[p];
//		if(mergePalettes(set_palette, palette, set_palette))
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

//static bool extractPalettes(PaletteSet& out_palette_set, const Image& image)
//{
//	return iterateImageTiles(
//		image,
//		[&image, &out_palette_set](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
//		{
//			Palette tile_palette;
//			if(!extractTilePalette(tile_palette, tile_pixels, image.getWidth()))
//			{
//				cout << "Could not extract tile palette for tile (" << tile_column << "," << tile_row << ")" << endl;
//				return false;
//			}
//			if(!mergePaletteIntoSet(out_palette_set, tile_palette))
//			{
//				cout << "Could not merge palette for tile (" << tile_column << "," << tile_row << ")" << endl;
//				return false;
//			}
//			return true;
//		});
//}

