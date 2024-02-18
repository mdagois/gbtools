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

struct Profile
{
	struct
	{
		uint32_t base_index;
		uint32_t max_count;
		uint32_t color_max_count;
		bool insert_transparent_color;
		bool share_first_color;
	} palette;

	struct
	{
		uint32_t microtile_width;
		uint32_t microtile_height;
		TileRemoval tile_removal_max;
		uint32_t tile_max_count;
		uint32_t bank_max_count;
	} tileset;

	struct
	{
		uint32_t parameter_bit_depth;
		bool has_parameters;
		bool has_attributes;
	} tilemap;

	struct
	{
		bool is_bg;
		bool is_sprite;
	} data;
};

extern Profile PROFILE;

bool initializeProfile(Hardware hardware, DataType data_type);
bool isProfileValid();


////////////////////////////////////////////////////////////////////////////////

}

