#pragma once

#include <vector>

#include "color.h"
#include "constants.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

class Palette
{
public:
	static uint32_t getColorMaxCount();
	static void setColorMaxCount(uint32_t count);

public:
	Palette();
	virtual ~Palette();

	void push(ColorRGBA color);
	void clear();
	uint32_t size() const;
	bool contains(const Palette& sub_palette) const;
	const ColorRGBA operator[](int32_t index) const;
	uint8_t findColorIndex(ColorRGBA color) const;
	void makeFirstColor(ColorRGBA color);

private:
	static uint32_t s_color_max_count;

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

	void push(const Palette& palette);
	const Palette& operator[](int32_t index) const;
	uint32_t size() const;
	bool optimize(bool share_first_color);
	uint32_t findCompatiblePaletteIndex(const Palette& palette) const;

private:
	std::vector<Palette> m_palettes;
};

////////////////////////////////////////////////////////////////////////////////

}

