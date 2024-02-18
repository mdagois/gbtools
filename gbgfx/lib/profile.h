#pragma once

#include <cstdint>

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Profile
////////////////////////////////////////////////////////////////////////////////

enum Hardware : uint32_t
{
	kHardwareDmg,
	kHardwareCgb,
	kHardwareSgb,
	kHardwareSfc,
	kHardwareCount,
};

enum DataType : uint32_t
{
	kDataTypeBg,
	kDataTypeSprite8x8,
	kDataTypeSprite8x16,
	kDataTypeCount,
};

enum TileRemoval : uint32_t
{
	kTileRemovalNone,
	kTileRemovalDoubles,
	kTileRemovalFlips,
	kTileRemovalCount,
};

////////////////////////////////////////

bool initialize(Hardware hardware, DataType data_type);
bool isParameterValid();

////////////////////////////////////////

Hardware getTargetHardware();
DataType getDataType();
bool isSprite();
TileRemoval getTileRemovalMax();
uint32_t getPaletteBaseIndex();
uint32_t getPaletteMaxCount();
uint32_t getPaletteColorMaxCount();
uint32_t getTileMaxCount();
uint32_t getBankMaxCount();
bool getUseTransparentColor();

////////////////////////////////////////////////////////////////////////////////

}
