#pragma once

#include <cstdint>

namespace gfx {

////////////////////////////////////////////////////////////////////////////////

enum Hardware : uint32_t
{
	kHardware_Dmg_Background,
	kHardware_Dmg_Sprite,
	kHardware_Cgb_Background,
	kHardware_Cgb_Sprite,
	kHardware_Sgb_Background,
	kHardware_Sgb_Border,
	kHardware_Printer,
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

