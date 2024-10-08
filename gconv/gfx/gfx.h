#pragma once

//TODO Support printer packets (compressed or not)
//TODO Support forcing the palette set instead of extracting it

#include <vector>

#include "data/data.h"
#include "export/export.h"
#include "import/import.h"
#include "third_party/third_party.h"
#include "utils/utils.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////

bool initialize(Hardware hardware);

////////////////////////////////////////////////////////////////////////////////
// Import
////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set, DivisionInfo& out_division_info,
	const std::vector<Division>& divisions,
	TileRemoval tile_removal, const char* image_filename);
bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set, DivisionInfo& out_division_info,
	const std::vector<Division>& divisions,
	TileRemoval tile_removal, const Image& image);

bool extractTilemap(
	Tilemap& out_tilemap, DivisionInfo& out_division_info,
	const Tileset& tileset, const PaletteSet& palette_set,
	const std::vector<Division>& divisions,
	const char* image_filename);
bool extractTilemap(
	Tilemap& out_tilemap, DivisionInfo& out_division_info,
	const Tileset& tileset, const PaletteSet& palette_set,
	const std::vector<Division>& divisions,
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
// Info
////////////////////////////////////////////////////////////////////////////////

bool loadDivisionInfo(DivisionInfo& out_division_info, const char* input_filename);
bool writeDivisionInfo(const DivisionInfo& division_info, const char* output_filename);
bool printDivisionInfo(const DivisionInfo& division_info);

////////////////////////////////////////////////////////////////////////////////
// Debug
////////////////////////////////////////////////////////////////////////////////

uint32_t getBasicTileWidth();
bool writeTilesetToPNG(
	const Tileset& tileset, TileFlipType flip_type, const PaletteSet& palette_set,
	uint32_t tile_column_count, bool clear_doubles, const char* filename);
bool writePaletteSetToPNG(const PaletteSet& palette_set, const char* filename);

////////////////////////////////////////////////////////////////////////////////

}

