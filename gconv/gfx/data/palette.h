#pragma once

#include <vector>

#include "color.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

class Palette
{
public:
	Palette(bool insert_transparent_color);
	virtual ~Palette();

	void add(ColorRGBA color);
	void fill(ColorRGBA color, uint32_t max_color_count);
	void clear();

	uint32_t size() const;
	const ColorRGBA operator[](int32_t index) const;

	bool contains(const Palette& sub_palette) const;
	bool hasColor(ColorRGBA color) const;
	bool findColorIndex(uint8_t& out_color_index, ColorRGBA color) const;
	void makeFirstColor(ColorRGBA color);
	double getAverageLuminance() const;

	void loadRawPaletteData(const ColorRGBA* colors, uint32_t color_count);

private:
	void sort();

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

	uint32_t add(const Palette& palette);
	const Palette& operator[](int32_t index) const;
	Palette& operator[](int32_t index);
	uint32_t size() const;
	bool optimize(uint32_t palette_color_max_count, bool share_first_color, bool fill_palettes);
	bool findCompatiblePaletteIndex(uint32_t& out_palette_index, const Palette& palette) const;

	void loadRawPaletteData(const ColorRGBA* colors, uint32_t color_count);
	void lock();
	bool isLocked() const;

private:
	std::vector<Palette> m_palettes;
	bool m_isLocked;
};

////////////////////////////////////////////////////////////////////////////////

}

