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
	Palette(bool insert_transparent_color);
	virtual ~Palette();

	void add(ColorRGBA color);
	void clear();

	uint32_t size() const;
	const ColorRGBA operator[](int32_t index) const;

	bool contains(const Palette& sub_palette) const;
	bool findColorIndex(uint8_t& out_color_index, ColorRGBA color) const;
	void makeFirstColor(ColorRGBA color);

private:
	std::vector<ColorRGBA> m_colors;
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
	bool optimize(uint32_t palette_color_max_count, bool share_first_color);
	bool findCompatiblePaletteIndex(uint32_t& out_palette_index, const Palette& palette) const;

private:
	std::vector<Palette> m_palettes;
};

////////////////////////////////////////////////////////////////////////////////

}

