#pragma once

#include <vector>

#include "color.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

class Palette
{
public:
	Palette();
	virtual ~Palette();

	void add(ColorRGBA color);
	void clear();
	uint32_t size() const;
	bool contains(const Palette& sub_palette) const;
	const ColorRGBA operator[](int32_t index) const;
	bool findColorIndex(uint8_t& out_color_index, ColorRGBA color) const;
	void makeFirstColor(ColorRGBA color);

private:
	enum : uint32_t
	{
		kColorsPerPalette_Max = 16,
	};

private:
	ColorRGBA m_colors[kColorsPerPalette_Max];
	uint32_t m_color_count;
};

////////////////////////////////////////////////////////////////////////////////
// Palette set
////////////////////////////////////////////////////////////////////////////////

class PaletteSet
{
public:
	PaletteSet();
	virtual ~PaletteSet();

	void add(const Palette& palette);
	const Palette& operator[](int32_t index) const;
	uint32_t size() const;
	bool optimize();
	bool findCompatiblePaletteIndex(uint32_t& out_palette_index, const Palette& palette) const;

private:
	std::vector<Palette> m_palettes;
};

////////////////////////////////////////////////////////////////////////////////

}

