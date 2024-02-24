#pragma once

#include <cstdint>

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

enum Hardware : uint32_t
{
	kHardware_Dmg_Background,
	kHardware_Dmg_Sprite,
	kHardware_Cgb_Background,
	kHardware_Cgb_Sprite,
	kHardware_Sgb_Background,
	kHardware_Sgb_Border,
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

