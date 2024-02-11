#pragma once

#include "palette.h"
#include "tilemap.h"
#include "tileset.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	bool use_microtile_8x16, bool remove_doubles, bool remove_flips,
	const char* image_filename);

bool extractTilemap(
	Tilemap& out_tilemap,
	const Tileset& tileset, const PaletteSet& palette_set,
	bool use_flips,
	const char* image_filename);

////////////////////////////////////////////////////////////////////////////////
// Output
////////////////////////////////////////////////////////////////////////////////

bool exportPaletteSet(
	const PaletteSet& palette_set,
	const char* output_filename, bool use_header);
bool exportTileset(
	const Tileset& tileset,
	const char* output_filename, bool use_header);
bool exportTilemap(
	const Tilemap& tilemap,
	const char* indices_filename, const char* parameter_filename, bool use_header);

////////////////////////////////////////////////////////////////////////////////

}
