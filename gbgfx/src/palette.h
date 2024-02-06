#pragma once

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
	~Palette();

	void push(ColorRGBA color);
	uint32_t getColorCount() const;
	const ColorRGBA operator[](int32_t index) const;

private:
	ColorRGBA m_colors[kColorsPerPalette];
	uint32_t m_color_count;
};

//struct PaletteSet
//{
//	Palette palettes[kPaletteMaxCount];
//};


//static bool mergePalettes(Palette& out_palette, const Palette lhs, const Palette rhs)

//static bool mergePaletteIntoSet(PaletteSet& out_palette_set, const Palette palette)

//static bool extractPalettes(PaletteSet& out_palette_set, const Image& image)

