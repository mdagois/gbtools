#pragma once

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

enum
{
	kTileSize = 8,
	kPixelsPerTile = kTileSize * kTileSize,

	kColorsPerPalette = 4,
	kInvalidColorIndex = 0xFFU,
	kInvalidPaletteIndex = 0xFFFFFFFFU,

	kTilemapRowMaxCount = 32,
	kTilemapColumnMaxCount = 32,

	kIterateAllRows = 0,
};

////////////////////////////////////////////////////////////////////////////////

}

