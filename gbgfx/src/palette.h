#pragma once

#include <vector>
#include "color.h"

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum
{
	kColorsPerPalette = 4,
	kInvalidColorIndex = 0xFFU,
	kInvalidPaletteIndex = 0xFFFFFFFFU,
};

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
	void optimize();
	uint32_t findCompatiblePaletteIndex(const Palette& palette) const;

private:
	std::vector<Palette> m_palettes;
};

