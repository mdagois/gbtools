#pragma once

#include <cstdint>

namespace gfx {

////////////////////////////////////////////////////////////////////////////////

enum Hardware : uint32_t
{
	kHardware_Dmg_Background,
	kHardware_Dmg_Sprite,
	kHardware_Cgb_Background,
	kHardware_Cgb_Sprite_8x8,
	kHardware_Cgb_Sprite_8x16,
	kHardware_Sgb_Background,
	kHardware_Sgb_Border,
	kHardware_Dmg_Printer,
	kHardware_Vdp_Sprite_8x8,
	kHardware_Vdp_Sprite_16x16,
	kHardware_Vdp_Mode_1,
	kHardwareCount,
};

////////////////////////////////////////////////////////////////////////////////

enum TileRemoval
{
	kTileRemovalNone,
	kTileRemovalDoubles,
	kTileRemovalFlips,
};

////////////////////////////////////////////////////////////////////////////////

}

