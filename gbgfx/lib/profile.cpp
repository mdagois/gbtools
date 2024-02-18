#include <cassert>

#include "profile.h"
#include "log.h"

namespace gbgfx { namespace profile {

////////////////////////////////////////////////////////////////////////////////
// Profile
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////

Hardware hardware = kHardwareCount;
DataType data_type = kDataTypeCount;

////////////////////////////////////////

extern bool supports_flips = false;

uint32_t palette_base_index = 0;
uint32_t palette_max_count = 0;
uint32_t palette_color_max_count = 0;
bool insert_transparent_color = false;
bool palette_share_first_color = false;

TileRemoval tile_removal_max = kTileRemovalCount;
uint32_t tile_max_count = 0;
uint32_t bank_max_count = 0;

uint32_t tilemap_parameter_bit_depth = 0;
bool tilemap_has_parameters = false;
bool tilemap_has_attributes = false;

bool is_bg = false;
bool is_sprite = false;
uint32_t microtile_width = 0;
uint32_t microtile_height = 0;

////////////////////////////////////////

bool initialize(Hardware hardware, DataType data_type)
{
	assert(hardware < kHardwareCount);
	assert(data_type < kDataTypeCount);

	profile::hardware = hardware;
	profile::data_type = data_type;

	if( (hardware == kHardwareSfc || hardware == kHardwareSgb) &&
		data_type != kDataTypeBg)
	{
		GBGFX_LOG_ERROR("This hardware only supports the BG data type");
		return false;
	}

	switch(hardware)
	{
		case kHardwareDmg:
			profile::supports_flips = false;

			profile::palette_base_index = 0;
			profile::palette_max_count = profile::data_type == kDataTypeBg ? 1 : 2;
			profile::palette_color_max_count = 4;
			profile::insert_transparent_color = false;
			profile::palette_share_first_color = false;

			profile::tile_removal_max = kTileRemovalDoubles;
			profile::tile_max_count = 256;
			profile::bank_max_count = 1;

			profile::tilemap_parameter_bit_depth = 0;
			profile::tilemap_has_parameters = false;
			profile::tilemap_has_attributes = false;
			break;
		case kHardwareCgb:
			profile::supports_flips = true;

			profile::palette_base_index = 0;
			profile::palette_max_count = 8;
			profile::palette_color_max_count = 4;
			profile::insert_transparent_color = false;
			profile::palette_share_first_color = false;

			profile::tile_removal_max = kTileRemovalFlips;
			profile::tile_max_count = 512;
			profile::bank_max_count = 2;

			profile::tilemap_parameter_bit_depth = 8;
			profile::tilemap_has_parameters = true;
			profile::tilemap_has_attributes = false;
			break;
		case kHardwareSgb:
			profile::supports_flips = false;

			profile::palette_base_index = 0;
			profile::palette_max_count = 4;
			profile::palette_color_max_count = 4;
			profile::insert_transparent_color = false;
			profile::palette_share_first_color = true;

			profile::tile_removal_max = kTileRemovalDoubles;
			profile::tile_max_count = 256;
			profile::bank_max_count = 1;

			profile::tilemap_parameter_bit_depth = 0;
			profile::tilemap_has_parameters = false;
			profile::tilemap_has_attributes = true;
			break;
		case kHardwareSfc:
			profile::supports_flips = true;

			profile::palette_base_index = 4;
			profile::palette_max_count = 3;
			profile::palette_color_max_count = 16;
			profile::insert_transparent_color = true;
			profile::palette_share_first_color = false;

			profile::tile_removal_max = kTileRemovalFlips;
			profile::tile_max_count = 256;
			profile::bank_max_count = 1;

			profile::tilemap_parameter_bit_depth = 16;
			profile::tilemap_has_parameters = true;
			profile::tilemap_has_attributes = false;
			break;
		default:
			assert(false);
			break;
	}

	is_bg = data_type == kDataTypeBg;
	is_sprite = !is_bg;
	if(is_sprite)
	{
		switch(data_type)
		{
			case kDataTypeBg:
			case kDataTypeSprite8x8:
				microtile_width = 8;
				microtile_height = 8;
				break;
			case kDataTypeSprite8x16:
				microtile_width = 8;
				microtile_height = 16;
				break;
			default:
				assert(false);
				break;
		}
	}
	return true;
}

bool isValid()
{
	return hardware < kHardwareCount;
}

////////////////////////////////////////////////////////////////////////////////

} }

