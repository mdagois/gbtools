#pragma once

#include "image.h"
#include "palette.h"
#include "result.h"
#include "tileset.h"

Result extractTilePalette(Palette& out_tile_palette, const ImageTile& tile);
Result generateTile(Tile& out_tile, const ImageTile& image_tile, const PaletteSet& palette_set);
Result writeTilesetToPNG(const char* filename, uint32_t tile_column_count, const Tileset& tileset, const PaletteSet& palette_set);

