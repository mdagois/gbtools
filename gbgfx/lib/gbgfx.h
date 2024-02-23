#pragma once

//TODO Add divisions to CLI
//TODO Support OAM export
//TODO Support forcing the palette set instead of extracting it
//TODO Allow to set an output directory
//TODO Support short name for options

#include "data/palette.h"
#include "data/tilemap.h"
#include "data/tileset.h"
#include "enums.h"
#include "import/image.h"
#include "utils/log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////

bool initialize(Hardware hardware, Mode mode);

////////////////////////////////////////////////////////////////////////////////
// Import
////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	const Division* divisions, uint32_t division_count,
	TileRemoval tile_removal, const char* image_filename);
bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	const Division* divisions, uint32_t division_count,
	TileRemoval tile_removal, const Image& image);

bool extractTilemap(
	Tilemap& out_tilemap,
	const Tileset& tileset, const PaletteSet& palette_set,
	const Division* divisions, uint32_t division_count,
	const char* image_filename);
bool extractTilemap(
	Tilemap& out_tilemap,
	const Tileset& tileset, const PaletteSet& palette_set,
	const Division* divisions, uint32_t division_count,
	const Image& image);

////////////////////////////////////////////////////////////////////////////////
// Export
////////////////////////////////////////////////////////////////////////////////

bool exportPaletteSet(
	const PaletteSet& palette_set, bool use_header,
	const char* output_filename);
bool exportTileset(
	const Tileset& tileset, bool use_header,
	const char* output_filename);
bool exportTilemap(
	const Tilemap& tilemap,
	bool use_header, bool use_8800_addressing_mode,
	uint8_t palette_index_offset, uint8_t tile_index_offset,
	const char* indices_filename, const char* parameter_filename);

////////////////////////////////////////////////////////////////////////////////
// Debug
////////////////////////////////////////////////////////////////////////////////

bool writeTilesetToPNG(
	const Tileset& tileset, TileFlipType flip_type, const PaletteSet& palette_set,
	uint32_t tile_column_count, bool clear_doubles, const char* filename);
bool writePaletteSetToPNG(const PaletteSet& palette_set, const char* filename);

////////////////////////////////////////////////////////////////////////////////

}

