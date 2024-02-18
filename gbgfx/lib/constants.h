#pragma once

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

enum
{
	kTileSize = 8,
	kPixelsPerTile = kTileSize * kTileSize,

	//kColorsPerPalette_GB = 4,
	//kColorsPerPalette_SFC = 16,
	kColorsPerPalette_Max = 16,

	kInvalidColorIndex = 0xFFU,
	kInvalidPaletteIndex = 0xFFFFFFFFU,
	kPaletteMaxCount = 8,

	kTilesPerBank = 256,

	kTilemapRowMaxCount = 32,
	kTilemapColumnMaxCount = 32,

	kIterateAllRows = 0,

	kSharedColorAlpha = 0,
};

////////////////////////////////////////////////////////////////////////////////

}

