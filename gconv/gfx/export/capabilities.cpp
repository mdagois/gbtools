#include <cassert>

#include "capabilities.h"
#include "utils/log.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////

static Hardware s_hardware = kHardwareCount;

static Capabilities s_capabilities = {};

////////////////////////////////////////////////////////////////////////////////

bool initializeCapabilities(Hardware hardware)
{
	assert(hardware < kHardwareCount);

	s_hardware = hardware;

	switch(hardware)
	{
		case kHardware_Dmg_Background:
			s_capabilities.palette.base_index = 0;
			s_capabilities.palette.max_count = 1;
			s_capabilities.palette.color_max_count = 4;
			s_capabilities.palette.insert_transparent_color = false;
			s_capabilities.palette.share_first_color = false;

			s_capabilities.tileset.color_index_format = kFormat_COL2222;
			s_capabilities.tileset.basic_tile_width = 8;
			s_capabilities.tileset.basic_tile_height = 8;
			s_capabilities.tileset.tile_group_size = 1;
			s_capabilities.tileset.tile_max_count = 256;
			s_capabilities.tileset.tiles_per_bank = 256;
			s_capabilities.tileset.bank_max_count = 1;
			s_capabilities.tileset.supports_tile_removal = true;

			s_capabilities.tilemap.enabled = true;
			s_capabilities.tilemap.tile_index_format = kFormat_IDX8;
			s_capabilities.tilemap.tile_parameter_format = kFormat_None;
			s_capabilities.tilemap.supports_tile_flips = false;

			s_capabilities.sprite.enabled = false;
			s_capabilities.sprite.supports_tile_flips = false;
			break;
		case kHardware_Dmg_Sprite:
			s_capabilities.palette.base_index = 0;
			s_capabilities.palette.max_count = 2;
			s_capabilities.palette.color_max_count = 4;
			s_capabilities.palette.insert_transparent_color = true;
			s_capabilities.palette.share_first_color = false;

			s_capabilities.tileset.color_index_format = kFormat_COL2222;
			s_capabilities.tileset.basic_tile_width = 8;
			s_capabilities.tileset.basic_tile_height = 8;
			s_capabilities.tileset.tile_group_size = 1;
			s_capabilities.tileset.tile_max_count = 0xFFFFFFFFU;
			s_capabilities.tileset.tiles_per_bank = 0xFFFFFFFFU;
			s_capabilities.tileset.bank_max_count = 1;
			s_capabilities.tileset.supports_tile_removal = true;

			s_capabilities.tilemap.enabled = false;
			s_capabilities.tilemap.tile_index_format = kFormat_None;
			s_capabilities.tilemap.tile_parameter_format = kFormat_None;
			s_capabilities.tilemap.supports_tile_flips = false;

			s_capabilities.sprite.enabled = true;
			s_capabilities.sprite.supports_tile_flips = true;
			break;
		case kHardware_Cgb_Background:
			s_capabilities.palette.base_index = 0;
			s_capabilities.palette.max_count = 8;
			s_capabilities.palette.color_max_count = 4;
			s_capabilities.palette.insert_transparent_color = false;
			s_capabilities.palette.share_first_color = false;

			s_capabilities.tileset.color_index_format = kFormat_COL2222;
			s_capabilities.tileset.basic_tile_width = 8;
			s_capabilities.tileset.basic_tile_height = 8;
			s_capabilities.tileset.tile_group_size = 1;
			s_capabilities.tileset.tile_max_count = 512;
			s_capabilities.tileset.tiles_per_bank = 256;
			s_capabilities.tileset.bank_max_count = 2;
			s_capabilities.tileset.supports_tile_removal = true;

			s_capabilities.tilemap.enabled = true;
			s_capabilities.tilemap.tile_index_format = kFormat_IDX8;
			s_capabilities.tilemap.tile_parameter_format = kFormat_PAL3_BNK1_X1_FLP2_PRI1;
			s_capabilities.tilemap.supports_tile_flips = true;

			s_capabilities.sprite.enabled = false;
			s_capabilities.sprite.supports_tile_flips = false;
			break;
		case kHardware_Cgb_Sprite_8x8:
		case kHardware_Cgb_Sprite_8x16:
			s_capabilities.palette.base_index = 0;
			s_capabilities.palette.max_count = 8;
			s_capabilities.palette.color_max_count = 4;
			s_capabilities.palette.insert_transparent_color = false;
			s_capabilities.palette.share_first_color = false;

			s_capabilities.tileset.color_index_format = kFormat_COL2222;
			s_capabilities.tileset.basic_tile_width = 8;
			s_capabilities.tileset.basic_tile_height = 8;
			s_capabilities.tileset.tile_group_size = hardware == kHardware_Cgb_Sprite_8x16 ? 2 : 1;
			s_capabilities.tileset.tile_max_count = 0xFFFFFFFFU;
			s_capabilities.tileset.tiles_per_bank = 0xFFFFFFFFU;
			s_capabilities.tileset.bank_max_count = 2;
			s_capabilities.tileset.supports_tile_removal = true;

			s_capabilities.tilemap.enabled = false;
			s_capabilities.tilemap.tile_index_format = kFormat_None;
			s_capabilities.tilemap.tile_parameter_format = kFormat_None;
			s_capabilities.tilemap.supports_tile_flips = false;

			s_capabilities.sprite.enabled = true;
			s_capabilities.sprite.supports_tile_flips = true;
			break;
		case kHardware_Sgb_Background:
			s_capabilities.palette.base_index = 0;
			s_capabilities.palette.max_count = 4;
			s_capabilities.palette.color_max_count = 4;
			s_capabilities.palette.insert_transparent_color = false;
			s_capabilities.palette.share_first_color = true;

			s_capabilities.tileset.color_index_format = kFormat_COL2222;
			s_capabilities.tileset.basic_tile_width = 8;
			s_capabilities.tileset.basic_tile_height = 8;
			s_capabilities.tileset.tile_group_size = 1;
			s_capabilities.tileset.tile_max_count = 256;
			s_capabilities.tileset.tiles_per_bank = 256;
			s_capabilities.tileset.bank_max_count = 1;
			s_capabilities.tileset.supports_tile_removal = true;

			s_capabilities.tilemap.enabled = true;
			s_capabilities.tilemap.tile_index_format = kFormat_IDX8;
			s_capabilities.tilemap.tile_parameter_format = kFormat_PAL2222;
			s_capabilities.tilemap.supports_tile_flips = false;

			s_capabilities.sprite.enabled = false;
			s_capabilities.sprite.supports_tile_flips = false;
			break;
		case kHardware_Sgb_Border:
			s_capabilities.palette.base_index = 4;
			s_capabilities.palette.max_count = 3;
			s_capabilities.palette.color_max_count = 16;
			s_capabilities.palette.insert_transparent_color = true;
			s_capabilities.palette.share_first_color = false;

			s_capabilities.tileset.color_index_format = kFormat_COL2222_COL2222_INTERLEAVED;
			s_capabilities.tileset.basic_tile_width = 8;
			s_capabilities.tileset.basic_tile_height = 8;
			s_capabilities.tileset.tile_group_size = 1;
			s_capabilities.tileset.tile_max_count = 256;
			s_capabilities.tileset.tiles_per_bank = 256;
			s_capabilities.tileset.bank_max_count = 1;
			s_capabilities.tileset.supports_tile_removal = true;

			s_capabilities.tilemap.enabled = true;
			s_capabilities.tilemap.tile_index_format = kFormat_None;
			s_capabilities.tilemap.tile_parameter_format = kFormat_IDX8_X2_PAL3_X1_FLP2;
			s_capabilities.tilemap.supports_tile_flips = true;

			s_capabilities.sprite.enabled = false;
			s_capabilities.sprite.supports_tile_flips = false;
			break;
		case kHardware_Printer:
			s_capabilities.palette.base_index = 0;
			s_capabilities.palette.max_count = 1;
			s_capabilities.palette.color_max_count = 4;
			s_capabilities.palette.insert_transparent_color = false;
			s_capabilities.palette.share_first_color = false;

			s_capabilities.tileset.color_index_format = kFormat_COL2222;
			s_capabilities.tileset.basic_tile_width = 8;
			s_capabilities.tileset.basic_tile_height = 8;
			s_capabilities.tileset.tile_group_size = 1;
			s_capabilities.tileset.tile_max_count = 0xFFFFFFFFU;
			s_capabilities.tileset.tiles_per_bank = 0xFFFFFFFFU;
			s_capabilities.tileset.bank_max_count = 1;
			s_capabilities.tileset.supports_tile_removal = false;

			s_capabilities.tilemap.enabled = false;
			s_capabilities.tilemap.tile_index_format = kFormat_None;
			s_capabilities.tilemap.tile_parameter_format = kFormat_None;
			s_capabilities.tilemap.supports_tile_flips = false;

			s_capabilities.sprite.enabled = false;
			s_capabilities.sprite.supports_tile_flips = false;
			break;
		default:
			assert(false);
			break;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool areCapabilitiesInitialized()
{
	return s_hardware < kHardwareCount;
}

////////////////////////////////////////////////////////////////////////////////

const Capabilities& getCapabilities()
{
	if(!areCapabilitiesInitialized())
	{
		GFX_LOG_ERROR("Capabilities are not initialized");
	}
	return s_capabilities;
}

////////////////////////////////////////////////////////////////////////////////

}

