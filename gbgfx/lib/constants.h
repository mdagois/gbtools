#pragma once

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

enum
{
	kTileSize = 8,
	kPixelsPerTile = kTileSize * kTileSize,

	kInvalidColorIndex = 0xFFU,
	kInvalidPaletteIndex = 0xFFFFFFFFU,

	kTilesPerBank = 256,

	kTilemapRowMaxCount = 32,
	kTilemapColumnMaxCount = 32,

	kIterateAllRows = 0,

	kSharedColorAlpha = 0,
};

////////////////////////////////////////////////////////////////////////////////

}

