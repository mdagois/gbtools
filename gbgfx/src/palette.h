#pragma once

#include <vector>
#include "color.h"

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kColorsPerPalette = 4,
//	kPaletteMaxCount = 8,
//	kVramBankCount = 2,
//	kTilesPerVramBank = 256,
//	kTilesMaxCount = kTilesPerVramBank * kVramBankCount,
//	kTilemapIndexMaxCount = 1024,

//	kColorIndex_Invalid = 0xFFFFFFFFU,
//	kPaletteIndex_Invalid = 0xFFFFFFFFU,
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
	uint32_t size() const;
	const Palette& operator[](int32_t index) const;
	void optimize();

private:
	std::vector<Palette> m_palettes;
};

