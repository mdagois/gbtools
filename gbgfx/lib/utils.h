#pragma once

#include "image.h"
#include "palette.h"
#include "tileset.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

bool extractTilePalette(Palette& out_tile_palette, const ImageTile& tile);
bool generateTile(Tile& out_tile, const ImageTile& image_tile, const PaletteSet& palette_set);

////////////////////////////////////////////////////////////////////////////////

}

