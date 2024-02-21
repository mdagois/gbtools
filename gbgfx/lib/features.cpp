#include <cassert>

#include "features.h"
#include "log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

static Hardware s_hardware = kHardwareCount;
static Mode s_mode = kModeCount;

static Features s_features = {};

////////////////////////////////////////////////////////////////////////////////

bool initialize(Hardware hardware, Mode mode)
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

			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 256;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;

			s_features.tilemap.index_format = kFormat_IDX8;
			s_features.tilemap.parameter_format = kFormat_None;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.extended_tile_width = (mode == kModeBg) ? 0 : 8;
			s_features.sprite.extended_tile_height = (mode == kModeBg) ? 0 : (mode == kModeSprite8x16 ? 16 : 8);
			break;
		case kHardwareCgb:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 8;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = false;

			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 512;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 2;

			s_features.tilemap.index_format = kFormat_IDX8;
			s_features.tilemap.parameter_format = kFormat_PAL3_BNK1_X1_FLP2_PRI1;
			s_features.tilemap.supports_tile_flips = true;

			s_features.sprite.extended_tile_width = (mode == kModeBg) ? 0 : 8;
			s_features.sprite.extended_tile_height = (mode == kModeBg) ? 0 : (mode == kModeSprite8x16 ? 16 : 8);
			break;
		case kHardwareSgb:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 4;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = true;

			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 256;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;

			s_features.tilemap.index_format = kFormat_IDX8;
			s_features.tilemap.parameter_format = kFormat_PAL2_PAL2_PAL2_PAL2;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.extended_tile_width = (mode == kModeBg) ? 0 : 8;
			s_features.sprite.extended_tile_height = (mode == kModeBg) ? 0 : (mode == kModeSprite8x16 ? 16 : 8);
			break;
		case kHardwareSfc:
			s_features.palette.base_index = 4;
			s_features.palette.max_count = 3;
			s_features.palette.color_max_count = 16;
			s_features.palette.insert_transparent_color = true;
			s_features.palette.share_first_color = false;

			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 256;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;

			s_features.tilemap.index_format = kFormat_None;
			s_features.tilemap.parameter_format = kFormat_IDX8_X2_PAL3_X1_FLP2;
			s_features.tilemap.supports_tile_flips = true;

			s_features.sprite.extended_tile_width = (mode == kModeBg) ? 0 : 8;
			s_features.sprite.extended_tile_height = (mode == kModeBg) ? 0 : (mode == kModeSprite8x16 ? 16 : 8);
			break;
		default:
			assert(false);
			break;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool isInitialized()
{
	return s_hardware < kHardwareCount;
}

////////////////////////////////////////////////////////////////////////////////

const Features& getFeatures()
{
	return s_features;
}

////////////////////////////////////////////////////////////////////////////////

}

