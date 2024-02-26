#include <cassert>

#include "features.h"
#include "utils/log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

static Hardware s_hardware = kHardwareCount;

static Features s_features = {};

////////////////////////////////////////////////////////////////////////////////

bool initializeFeatures(Hardware hardware)
{
	assert(hardware < kHardwareCount);

	s_hardware = hardware;

	switch(hardware)
	{
		case kHardware_Dmg_Background:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 1;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 256;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;
			s_features.tileset.supports_tile_removal = true;

			s_features.tilemap.enabled = true;
			s_features.tilemap.tile_index_format = kFormat_IDX8;
			s_features.tilemap.tile_parameter_format = kFormat_None;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.enabled = false;
			s_features.sprite.supports_tile_flips = false;
			break;
		case kHardware_Dmg_Sprite:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 2;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = true;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 0xFFFFFFFFU;
			s_features.tileset.tiles_per_bank = 0xFFFFFFFFU;
			s_features.tileset.bank_max_count = 1;
			s_features.tileset.supports_tile_removal = true;

			s_features.tilemap.enabled = false;
			s_features.tilemap.tile_index_format = kFormat_None;
			s_features.tilemap.tile_parameter_format = kFormat_None;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.enabled = true;
			s_features.sprite.supports_tile_flips = true;
			break;
		case kHardware_Cgb_Background:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 8;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 512;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 2;
			s_features.tileset.supports_tile_removal = true;

			s_features.tilemap.enabled = true;
			s_features.tilemap.tile_index_format = kFormat_IDX8;
			s_features.tilemap.tile_parameter_format = kFormat_PAL3_BNK1_X1_FLP2_PRI1;
			s_features.tilemap.supports_tile_flips = true;

			s_features.sprite.enabled = false;
			s_features.sprite.supports_tile_flips = false;
			break;
		case kHardware_Cgb_Sprite:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 8;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 0xFFFFFFFFU;
			s_features.tileset.tiles_per_bank = 0xFFFFFFFFU;
			s_features.tileset.bank_max_count = 2;
			s_features.tileset.supports_tile_removal = true;

			s_features.tilemap.enabled = false;
			s_features.tilemap.tile_index_format = kFormat_None;
			s_features.tilemap.tile_parameter_format = kFormat_None;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.enabled = true;
			s_features.sprite.supports_tile_flips = true;
			break;
		case kHardware_Sgb_Background:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 4;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = true;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 256;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;
			s_features.tileset.supports_tile_removal = true;

			s_features.tilemap.enabled = true;
			s_features.tilemap.tile_index_format = kFormat_IDX8;
			s_features.tilemap.tile_parameter_format = kFormat_PAL2222;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.enabled = false;
			s_features.sprite.supports_tile_flips = false;
			break;
		case kHardware_Sgb_Border:
			s_features.palette.base_index = 4;
			s_features.palette.max_count = 3;
			s_features.palette.color_max_count = 16;
			s_features.palette.insert_transparent_color = true;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222_COL2222_INTERLEAVED;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 256;
			s_features.tileset.tiles_per_bank = 256;
			s_features.tileset.bank_max_count = 1;
			s_features.tileset.supports_tile_removal = true;

			s_features.tilemap.enabled = true;
			s_features.tilemap.tile_index_format = kFormat_None;
			s_features.tilemap.tile_parameter_format = kFormat_IDX8_X2_PAL3_X1_FLP2;
			s_features.tilemap.supports_tile_flips = true;

			s_features.sprite.enabled = false;
			s_features.sprite.supports_tile_flips = false;
			break;
		case kHardware_Printer:
			s_features.palette.base_index = 0;
			s_features.palette.max_count = 1;
			s_features.palette.color_max_count = 4;
			s_features.palette.insert_transparent_color = false;
			s_features.palette.share_first_color = false;

			s_features.tileset.color_index_format = kFormat_COL2222;
			s_features.tileset.basic_tile_width = 8;
			s_features.tileset.basic_tile_height = 8;
			s_features.tileset.tile_max_count = 0xFFFFFFFFU;
			s_features.tileset.tiles_per_bank = 0xFFFFFFFFU;
			s_features.tileset.bank_max_count = 1;
			s_features.tileset.supports_tile_removal = false;

			s_features.tilemap.enabled = false;
			s_features.tilemap.tile_index_format = kFormat_None;
			s_features.tilemap.tile_parameter_format = kFormat_None;
			s_features.tilemap.supports_tile_flips = false;

			s_features.sprite.enabled = false;
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

