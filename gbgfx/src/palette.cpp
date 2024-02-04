#include "palette.h"

Palette::Palette()
{
	for(int32_t i = 0; i < kColorsPerPalette; ++i)
	{
		colors[i] = kBGR555_Invalid;
	}
}

//struct PaletteSet
//{
//	Palette palettes[kPaletteMaxCount];
//};
//
//static bool extractTilePalette(Palette& out_tile_palette, const ColorRGBA* pixels, uint32_t row_pitch)
//{
//	set<ColorBGR555> colors;
//	for(uint32_t j = 0; j < kTileSize; ++j)
//	{
//		const ColorRGBA* row_pixels = pixels + (j * row_pitch);
//		for(uint32_t i = 0; i < kTileSize; ++i)
//		{
//			colors.insert(convertColor(row_pixels[i]));
//		}
//	}
//	if(colors.size() > kColorsPerPalette)
//	{
//		cout << "Too many colors (" << colors.size() << ") in tile" << endl;
//		return false;
//	}
//
//	uint32_t c = 0;
//	for(ColorBGR555 color : colors)
//	{
//		out_tile_palette.colors[c] = color;
//		++c;
//	}
//	sortColors(out_tile_palette.colors, c);
//
//	return true;
//}
//
//static bool mergePalettes(Palette& out_palette, const Palette lhs, const Palette rhs)
//{
//	set<ColorBGR555> colors;
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
//	for(ColorBGR555 color : colors)
//	{
//		out_palette.colors[c] = color;
//		++c;
//	}
//	sortColors(out_palette.colors, c);
//	return true;
//}
//
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
//
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

