#pragma once

#include "image.h"
#include "palette.h"
#include "tilemap.h"
#include "tileset.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Hardware
////////////////////////////////////////////////////////////////////////////////

enum Hardware : uint32_t
{
	kHardwareDmg,
	kHardwareCgb,
	kHardwareSgb,
	kHardwareSfc,
	kHardwareCount,
};

enum DataType : uint32_t
{
	kDataTypeBg,
	kDataTypeSprite8x8,
	kDataTypeSprite8x16,
	kDataTypeCount,
};

enum TileRemoval : uint32_t
{
	kTileRemovalNone,
	kTileRemovalDoubles,
	kTileRemovalFlips,
	kTileRemovalCount,
};

////////////////////////////////////////

bool initialize(Hardware hardware, DataType data_type);

////////////////////////////////////////

Hardware getTargetHardware();
DataType getDataType();
bool isSprite();
TileRemoval getTileRemovalMax();
uint32_t getPaletteMaxCount();
uint32_t getPaletteColorMaxCount();
uint32_t getTileMaxCount();
bool getUseTransparentColor();

////////////////////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////////////////////

bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	bool skip_single_color_metatiles, TileRemoval tile_removal,
	const char* image_filename);
bool extractTileset(
	Tileset& out_tileset, PaletteSet& out_palette_set,
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	bool skip_single_color_metatiles, TileRemoval tile_removal,
	const Image& image);

bool extractTilemap(
	Tilemap& out_tilemap,
	const Tileset& tileset, const PaletteSet& palette_set,
	bool use_flips,
	const char* image_filename);
bool extractTilemap(
	Tilemap& out_tilemap,
	const Tileset& tileset, const PaletteSet& palette_set,
	bool use_flips,
	const Image& image);

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
	const char* indices_filename,
	const char* parameter_filename,
	const char* attributes_filename,
	bool use_header, uint8_t palette_index_offset, uint8_t tile_index_offset);

////////////////////////////////////////////////////////////////////////////////
// Debug
////////////////////////////////////////////////////////////////////////////////

bool writeTilesetToPNG(
	const char* filename, uint32_t tile_column_count,
	const Tileset& tileset, TileFlipType flip_type, const PaletteSet& palette_set,
	bool clear_doubles);
bool writePaletteSetToPNG(const char* filename, const PaletteSet& palette_set);

////////////////////////////////////////////////////////////////////////////////

}
