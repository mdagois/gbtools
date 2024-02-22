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

	// Tilemap indices
	kFormat_IDX8,
	// CGB tilemap parameters
	kFormat_PAL3_BNK1_X1_FLP2_PRI1,
	// SGB screen attributes
	kFormat_PAL2_PAL2_PAL2_PAL2,
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
		// Format of the index data
		Format index_format;
		// Format of the parameter data
		Format parameter_format;
		// The tilemap can use tile flips
		bool supports_tile_flips;
	} tilemap;

	struct
	{
		// Extended tile dimension for sprites
		uint32_t extended_tile_width;
		uint32_t extended_tile_height;
	} sprite;
};

////////////////////////////////////////////////////////////////////////////////

bool initializeFeatures(Hardware hardware, Mode mode);
bool areFeaturesInitialized();
const Features& getFeatures();

#define FEATURES gbgfx::getFeatures()

////////////////////////////////////////////////////////////////////////////////

}

