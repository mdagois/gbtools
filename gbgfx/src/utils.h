#pragma once

#include "image.h"
#include "palette.h"
#include "tileset.h"

Result extractTilePalette(Palette& out_tile_palette, const ImageTile& tile);
//TODO Replace bool with result
bool generateTile(Tile& out_tile, const ImageTile& image_tile, const PaletteSet& palette_set);

