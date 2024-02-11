#pragma once

#include <vector>

#include "color.h"
#include "constants.h"

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

class Palette
{
public:
	Palette();
	virtual ~Palette();

	void push(ColorRGBA color);
	void clear();
	uint32_t size() const;
	bool contains(const Palette& sub_palette) const;
	const ColorRGBA operator[](int32_t index) const;
	uint8_t findColorIndex(ColorRGBA color) const;
	void sortColors();

private:
	ColorRGBA m_colors[kColorsPerPalette];
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

	void push(const Palette& palette);
	const Palette& operator[](int32_t index) const;
	uint32_t size() const;
	void optimize(bool merge_palettes, bool sort_palette_colors);
	uint32_t findCompatiblePaletteIndex(const Palette& palette) const;

private:
	std::vector<Palette> m_palettes;
};

