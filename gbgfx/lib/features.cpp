#include <cassert>

#include "features.h"
#include "utils/log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

static Hardware s_hardware = kHardwareCount;
static Mode s_mode = kModeCount;

static Features s_features = {};

////////////////////////////////////////////////////////////////////////////////

bool initializeFeatures(Hardware hardware, Mode mode)
{
	assert(hardware < kHardwareCount);
	assert(mode < kModeCount);

	s_hardware = hardware;
	s_mode = mode;

	if( (hardware == kHardwareSfc || hardware == kHardwareSgb) &&
		mode != kModeBg)
	{
		GBGFX_LOG_ERROR("This hardware only supports the BG mode");
		return false;
	}

	switch(hardware)
	{
		case kHardwareDmg:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = (mode == kModeBg) ? 1 : 2;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = (mode == kModeBg) ? 256 : 0xFFFFFFFFU;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;

			s_features.tilemap.enabled = mode == kModeBg;
			s_features.tilemap.tile_index_format = kFormat_IDX8;
			s_features.tilemap.tile_parameter_format = kFormat_None;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.enabled = mode != kModeBg;
			s_features.sprite.extended_tile_width = 8;
			s_features.sprite.extended_tile_height = (mode == kModeSprite8x16) ? 16 : 8;
			s_features.sprite.supports_tile_flips = true;
			break;
		case kHardwareCgb:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 8;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = (mode == kModeBg) ? 512 : 0xFFFFFFFFU;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 2;

			s_features.tilemap.enabled = mode == kModeBg;
			s_features.tilemap.tile_index_format = kFormat_IDX8;
			s_features.tilemap.tile_parameter_format = kFormat_PAL3_BNK1_X1_FLP2_PRI1;
			s_features.tilemap.supports_tile_flips = true;

			s_features.sprite.enabled = mode != kModeBg;
			s_features.sprite.extended_tile_width =  8;
			s_features.sprite.extended_tile_height = (mode == kModeSprite8x16) ? 16 : 8;
			s_features.sprite.supports_tile_flips = true;
			break;
		case kHardwareSgb:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 4;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = true;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = (mode == kModeBg) ? 256 : 0xFFFFFFFFU;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;

			s_features.tilemap.enabled = mode == kModeBg;
			s_features.tilemap.tile_index_format = kFormat_IDX8;
			s_features.tilemap.tile_parameter_format = kFormat_PAL2222;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.enabled = mode != kModeBg;
			s_features.sprite.extended_tile_width = 8;
			s_features.sprite.extended_tile_height = (mode == kModeSprite8x16) ? 16 : 8;
			s_features.sprite.supports_tile_flips = false;
			break;
		case kHardwareSfc:
			s_features.palette.base_index = 4;
			s_features.palette.max_count = 3;
			s_features.palette.color_max_count = 16;
			s_features.palette.insert_transparent_color = true;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222_COL2222_INTERLEAVED;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = (mode == kModeBg) ? 256 : 0xFFFFFFFFU;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;

			s_features.tilemap.enabled = mode == kModeBg;
			s_features.tilemap.tile_index_format = kFormat_None;
			s_features.tilemap.tile_parameter_format = kFormat_IDX8_X2_PAL3_X1_FLP2;
			s_features.tilemap.supports_tile_flips = true;

			s_features.sprite.enabled = mode != kModeBg;
			s_features.sprite.extended_tile_width = 8;
			s_features.sprite.extended_tile_height = (mode == kModeSprite8x16) ? 16 : 8;
			s_features.sprite.supports_tile_flips = false;
			break;
		default:
			assert(false);
			break;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool areFeaturesInitialized()
{
	return s_hardware < kHardwareCount;
}

////////////////////////////////////////////////////////////////////////////////

const Features& getFeatures()
{
	if(!areFeaturesInitialized())
	{
		GBGFX_LOG_ERROR("Features are not initialized");
	}
	return s_features;
}

////////////////////////////////////////////////////////////////////////////////

}

