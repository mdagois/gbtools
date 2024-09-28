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

	Capabilities& cap = s_capabilities;
	switch(hardware)
	{
		case kHardware_Dmg_Background:
			cap.palette.base_index = 0;
			cap.palette.max_count = 1;
			cap.palette.color_max_count = 4;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = false;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL4x2;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = 1;
			cap.tileset.tile_max_count = 256;
			cap.tileset.tiles_per_bank = 256;
			cap.tileset.bank_max_count = 1;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = true;
			cap.tilemap.tile_index_format = kFormat_IDX8;
			cap.tilemap.tile_parameter_format = kFormat_None;
			cap.tilemap.supports_tile_flips = false;

			cap.sprite.enabled = false;
			cap.sprite.supports_tile_flips = false;
			break;
		case kHardware_Dmg_Sprite:
			cap.palette.base_index = 0;
			cap.palette.max_count = 2;
			cap.palette.color_max_count = 4;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = true;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL4x2;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = 1;
			cap.tileset.tile_max_count = 0xFFFFFFFFU;
			cap.tileset.tiles_per_bank = 0xFFFFFFFFU;
			cap.tileset.bank_max_count = 1;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = false;
			cap.tilemap.tile_index_format = kFormat_None;
			cap.tilemap.tile_parameter_format = kFormat_None;
			cap.tilemap.supports_tile_flips = false;

			cap.sprite.enabled = true;
			cap.sprite.supports_tile_flips = true;
			break;
		case kHardware_Cgb_Background:
			cap.palette.base_index = 0;
			cap.palette.max_count = 8;
			cap.palette.color_max_count = 4;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = false;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL4x2;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = 1;
			cap.tileset.tile_max_count = 512;
			cap.tileset.tiles_per_bank = 256;
			cap.tileset.bank_max_count = 2;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = true;
			cap.tilemap.tile_index_format = kFormat_IDX8;
			cap.tilemap.tile_parameter_format = kFormat_PAL3_BNK1_X1_FLP2_PRI1;
			cap.tilemap.supports_tile_flips = true;

			cap.sprite.enabled = false;
			cap.sprite.supports_tile_flips = false;
			break;
		case kHardware_Cgb_Sprite_8x8:
		case kHardware_Cgb_Sprite_8x16:
			cap.palette.base_index = 0;
			cap.palette.max_count = 8;
			cap.palette.color_max_count = 4;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = true;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL4x2;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = hardware == kHardware_Cgb_Sprite_8x16 ? 2 : 1;
			cap.tileset.tile_max_count = 0xFFFFFFFFU;
			cap.tileset.tiles_per_bank = 0xFFFFFFFFU;
			cap.tileset.bank_max_count = 2;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = false;
			cap.tilemap.tile_index_format = kFormat_None;
			cap.tilemap.tile_parameter_format = kFormat_None;
			cap.tilemap.supports_tile_flips = false;

			cap.sprite.enabled = true;
			cap.sprite.supports_tile_flips = true;
			break;
		case kHardware_Sgb_Background:
			cap.palette.base_index = 0;
			cap.palette.max_count = 4;
			cap.palette.color_max_count = 4;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = false;
			cap.palette.share_first_color = true;

			cap.tileset.color_index_format = kFormat_COL4x2;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = 1;
			cap.tileset.tile_max_count = 256;
			cap.tileset.tiles_per_bank = 256;
			cap.tileset.bank_max_count = 1;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = true;
			cap.tilemap.tile_index_format = kFormat_IDX8;
			cap.tilemap.tile_parameter_format = kFormat_PAL4x2;
			cap.tilemap.supports_tile_flips = false;

			cap.sprite.enabled = false;
			cap.sprite.supports_tile_flips = false;
			break;
		case kHardware_Sgb_Border:
			cap.palette.base_index = 4;
			cap.palette.max_count = 3;
			cap.palette.color_max_count = 16;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = true;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL4x2_COL4x2_INTERLEAVED;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = 1;
			cap.tileset.tile_max_count = 256;
			cap.tileset.tiles_per_bank = 256;
			cap.tileset.bank_max_count = 1;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = true;
			cap.tilemap.tile_index_format = kFormat_None;
			cap.tilemap.tile_parameter_format = kFormat_IDX8_X2_PAL3_X1_FLP2;
			cap.tilemap.supports_tile_flips = true;

			cap.sprite.enabled = false;
			cap.sprite.supports_tile_flips = false;
			break;
		case kHardware_Dmg_Printer:
			cap.palette.base_index = 0;
			cap.palette.max_count = 1;
			cap.palette.color_max_count = 4;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = false;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL4x2;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = 1;
			cap.tileset.tile_max_count = 0xFFFFFFFFU;
			cap.tileset.tiles_per_bank = 0xFFFFFFFFU;
			cap.tileset.bank_max_count = 1;
			cap.tileset.supports_tile_removal = false;

			cap.tilemap.enabled = false;
			cap.tilemap.tile_index_format = kFormat_None;
			cap.tilemap.tile_parameter_format = kFormat_None;
			cap.tilemap.supports_tile_flips = false;

			cap.sprite.enabled = false;
			cap.sprite.supports_tile_flips = false;
			break;
		case kHardware_Vdp_Sprite_8x8:
		case kHardware_Vdp_Sprite_16x16:
			cap.palette.base_index = 0;
			cap.palette.max_count = 16;
			cap.palette.color_max_count = 2;
			cap.palette.tiles_per_palette = 1;
			cap.palette.insert_transparent_color = true;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL8x1;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = hardware == kHardware_Vdp_Sprite_16x16 ? 4 : 1;
			cap.tileset.tile_max_count = 0xFFFFFFFFU;
			cap.tileset.tiles_per_bank = 0xFFFFFFFFU;
			cap.tileset.bank_max_count = 1;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = false;
			cap.tilemap.tile_index_format = kFormat_None;
			cap.tilemap.tile_parameter_format = kFormat_None;
			cap.tilemap.supports_tile_flips = false;

			cap.sprite.enabled = true;
			cap.sprite.supports_tile_flips = false;
			break;
		case kHardware_Vdp_Mode_1:
			cap.palette.base_index = 0;
			cap.palette.max_count = 32;
			cap.palette.color_max_count = 2;
			cap.palette.tiles_per_palette = 8;
			cap.palette.insert_transparent_color = false;
			cap.palette.share_first_color = false;

			cap.tileset.color_index_format = kFormat_COL8x1;
			cap.tileset.basic_tile_width = 8;
			cap.tileset.basic_tile_height = 8;
			cap.tileset.tile_group_size = 1;
			cap.tileset.tile_max_count = 256;
			cap.tileset.tiles_per_bank = 1;
			cap.tileset.bank_max_count = 1;
			cap.tileset.supports_tile_removal = true;

			cap.tilemap.enabled = true;
			cap.tilemap.tile_index_format = kFormat_IDX8;
			cap.tilemap.tile_parameter_format = kFormat_None;
			cap.tilemap.supports_tile_flips = false;

			cap.sprite.enabled = false;
			cap.sprite.supports_tile_flips = false;
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

