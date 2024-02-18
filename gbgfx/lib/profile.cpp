#include <cassert>

#include "profile.h"
#include "log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

static Hardware s_hardware = kHardwareCount;
static DataType s_data_type = kDataTypeCount;

Profile PROFILE = {0};

////////////////////////////////////////

bool initializeProfile(Hardware hardware, DataType data_type)
{
	assert(hardware < kHardwareCount);
	assert(data_type < kDataTypeCount);

	s_hardware = hardware;
	s_data_type = data_type;

	if( (hardware == kHardwareSfc || hardware == kHardwareSgb) &&
		data_type != kDataTypeBg)
	{
		GBGFX_LOG_ERROR("This hardware only supports the BG data type");
		return false;
	}

	switch(hardware)
	{
		case kHardwareDmg:
			PROFILE.palette.base_index = 0;
			PROFILE.palette.max_count = data_type == kDataTypeBg ? 1 : 2;
			PROFILE.palette.color_max_count = 4;
			PROFILE.palette.insert_transparent_color = false;
			PROFILE.palette.share_first_color = false;

			PROFILE.tileset.tile_removal_max = kTileRemovalDoubles;
			PROFILE.tileset.tile_max_count = 256;
			PROFILE.tileset.bank_max_count = 1;

			PROFILE.tilemap.parameter_bit_depth = 0;
			PROFILE.tilemap.has_parameters = false;
			PROFILE.tilemap.has_attributes = false;
			break;
		case kHardwareCgb:
			PROFILE.palette.base_index = 0;
			PROFILE.palette.max_count = 8;
			PROFILE.palette.color_max_count = 4;
			PROFILE.palette.insert_transparent_color = false;
			PROFILE.palette.share_first_color = false;

			PROFILE.tileset.tile_removal_max = kTileRemovalFlips;
			PROFILE.tileset.tile_max_count = 512;
			PROFILE.tileset.bank_max_count = 2;

			PROFILE.tilemap.parameter_bit_depth = 8;
			PROFILE.tilemap.has_parameters = true;
			PROFILE.tilemap.has_attributes = false;
			break;
		case kHardwareSgb:
			PROFILE.palette.base_index = 0;
			PROFILE.palette.max_count = 4;
			PROFILE.palette.color_max_count = 4;
			PROFILE.palette.insert_transparent_color = false;
			PROFILE.palette.share_first_color = true;

			PROFILE.tileset.tile_removal_max = kTileRemovalDoubles;
			PROFILE.tileset.tile_max_count = 256;
			PROFILE.tileset.bank_max_count = 1;

			PROFILE.tilemap.parameter_bit_depth = 0;
			PROFILE.tilemap.has_parameters = false;
			PROFILE.tilemap.has_attributes = true;
			break;
		case kHardwareSfc:
			PROFILE.palette.base_index = 4;
			PROFILE.palette.max_count = 3;
			PROFILE.palette.color_max_count = 16;
			PROFILE.palette.insert_transparent_color = true;
			PROFILE.palette.share_first_color = false;

			PROFILE.tileset.tile_removal_max = kTileRemovalFlips;
			PROFILE.tileset.tile_max_count = 256;
			PROFILE.tileset.bank_max_count = 1;

			PROFILE.tilemap.parameter_bit_depth = 16;
			PROFILE.tilemap.has_parameters = true;
			PROFILE.tilemap.has_attributes = false;
			break;
		default:
			assert(false);
			break;
	}

	PROFILE.data.is_bg = data_type == kDataTypeBg;
	PROFILE.data.is_sprite = !PROFILE.data.is_bg;
	switch(data_type)
	{
		case kDataTypeBg:
		case kDataTypeSprite8x8:
			PROFILE.tileset.microtile_width = 8;
			PROFILE.tileset.microtile_height = 8;
			break;
		case kDataTypeSprite8x16:
			PROFILE.tileset.microtile_width = 8;
			PROFILE.tileset.microtile_height = 16;
			break;
		default:
			assert(false);
			break;
	}

	return true;
}

bool isProfileValid()
{
	return s_hardware < kHardwareCount;
}

////////////////////////////////////////////////////////////////////////////////

}

