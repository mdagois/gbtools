#pragma once

#include <cstdint>

#include "enums.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Hardware capabilities
////////////////////////////////////////////////////////////////////////////////

enum Format : uint32_t
{
	kFormat_None,

	// 1-bit color indices
	kFormat_COL8x1,
	// 2-bit color indices
	kFormat_COL4x2,
	// 4-bit interleaved color indices
	kFormat_COL4x2_COL4x2_INTERLEAVED,

	// Tilemap indices
	kFormat_IDX8,

	// CGB tilemap parameters
	kFormat_PAL3_BNK1_X1_FLP2_PRI1,
	// SGB screen attributes
	kFormat_PAL4x2,
	// SFC tilemap parameters
	kFormat_IDX8_X2_PAL3_X1_FLP2,

	kFormat_Count,
};

////////////////////////////////////////////////////////////////////////////////

struct Capabilities
{
	struct
	{
		// Index of the first palette
		uint32_t base_index;
		// Maximum number of palettes
		uint32_t max_count;
		// Maximum number of colors per palette
		uint32_t color_max_count;
		// Number of tiles assigned per palette
		uint32_t tiles_per_palette;
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
		// Size of a tile group (consecutive tiles that use the same palette)
		uint32_t tile_group_size;
		// Maximum number of tiles
		uint32_t tile_max_count;
		// Number of tiles per bank
		uint32_t tiles_per_bank;
		// Maximum number of tile banks
		uint32_t bank_max_count;
		// Tiles can be optimized out of the tileset
		bool supports_tile_removal;
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
		// Sprites can use tile flips
		bool supports_tile_flips;
	} sprite;
};

////////////////////////////////////////////////////////////////////////////////

bool initializeCapabilities(Hardware hardware);
bool areCapabilitiesInitialized();
const Capabilities& getCapabilities();

#define CAPS gfx::getCapabilities()

////////////////////////////////////////////////////////////////////////////////

}

