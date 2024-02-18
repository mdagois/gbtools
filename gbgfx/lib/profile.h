#pragma once

#include <cstdint>

namespace gbgfx {

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

////////////////////////////////////////////////////////////////////////////////
// Profile
////////////////////////////////////////////////////////////////////////////////

namespace profile
{
	// hardware
	extern bool supports_flips;

	// palette
	extern uint32_t palette_base_index;
	extern uint32_t palette_max_count;
	extern uint32_t palette_color_max_count;
	extern bool insert_transparent_color;
	extern bool palette_share_first_color;

	// tileset
	extern TileRemoval tile_removal_max;
	extern uint32_t tile_max_count;
	extern uint32_t bank_max_count;

	// tilemap
	extern uint32_t tilemap_parameter_bit_depth;
	extern bool tilemap_has_parameters;
	extern bool tilemap_has_attributes;

	// misc
	extern bool is_bg;
	extern bool is_sprite;
	extern uint32_t microtile_width;
	extern uint32_t microtile_height;

	// function
	bool initialize(Hardware hardware, DataType data_type);
	bool isValid();
}

////////////////////////////////////////////////////////////////////////////////

}

