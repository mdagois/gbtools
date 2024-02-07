#pragma once

#include "image.h"
#include "palette.h"
#include "tileset.h"

Result extractTilePalette(Palette& out_tile_palette, const ImageTile& tile);
Tile convert(const ImageTile& image_tile);

