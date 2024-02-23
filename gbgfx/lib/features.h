#pragma once

#include <cstdint>

#include "enums.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Features
////////////////////////////////////////////////////////////////////////////////

enum Format : uint32_t
{
	kFormat_None,

	// 2-bit color indices
	kFormat_COL2222,
	// 4-bit interleave color indices
	kFormat_COL2222_COL2222_INTERLEAVED,

	// Tilemap indices
	kFormat_IDX8,

	// CGB tilemap parameters
	kFormat_PAL3_BNK1_X1_FLP2_PRI1,
	// SGB screen attributes
	kFormat_PAL2222,
	// SFC tilemap parameters
	kFormat_IDX8_X2_PAL3_X1_FLP2,

	kFormat_Count,
};

////////////////////////////////////////////////////////////////////////////////

struct Features
{
	struct
	{
		// Index of the first palette
		uint32_t base_index;
		// Maximum number of palettes
		uint32_t max_count;
		// Maximum number of colors per palette
		uint32_t color_max_count;
		// The first color of all palette is transparent
		bool insert_transparent_color;
		// The first color of all palettes is shared
		bool share_first_color;
	} palette;

	struct
	{
		// Color index format
		Format color_index_format;
		// Basic tile dimension
		uint32_t basic_tile_width;
		uint32_t basic_tile_height;
		// Maximum number of tiles
		uint32_t tile_max_count;
		// Number of tiles per bank
		uint32_t tiles_per_bank;
		// Maximum number of tile banks
		uint32_t bank_max_count;
	} tileset;

	struct
	{
		// Tilemap output is enabled
		bool enabled;
		// Format of the tile index
		Format tile_index_format;
		// Format of the tile parameter
		Format tile_parameter_format;
		// Tilemaps can use tile flips
		bool supports_tile_flips;
	} tilemap;

	struct
	{
		// Sprite output is enabled
		bool enabled;
		// Extended tile dimension for sprites
		uint32_t extended_tile_width;
		uint32_t extended_tile_height;
		// Sprites can use tile flips
		bool supports_tile_flips;
	} sprite;
};

////////////////////////////////////////////////////////////////////////////////

bool initializeFeatures(Hardware hardware, Mode mode);
bool areFeaturesInitialized();
const Features& getFeatures();

#define FEATURES gbgfx::getFeatures()

////////////////////////////////////////////////////////////////////////////////

}

