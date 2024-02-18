#include <cassert>

#include "constants.h"
#include "profile.h"
#include "log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Profile
////////////////////////////////////////////////////////////////////////////////

struct Parameter
{
	Hardware hardware = kHardwareCount;
	DataType data_type = kDataTypeCount;
	TileRemoval tile_removal_max = kTileRemovalCount;
	uint32_t palette_base_index = 0;
	uint32_t palette_max_count = 0;
	uint32_t palette_color_max_count = 0;
	uint32_t tile_max_count = 0;
	uint32_t bank_max_count = 0;
	bool use_transparent_color = false;
};
static Parameter s_parameters;

////////////////////////////////////////

bool initialize(Hardware hardware, DataType data_type)
{
	assert(hardware < kHardwareCount);
	assert(data_type < kDataTypeCount);

	s_parameters.hardware = hardware;
	s_parameters.data_type = data_type;

	if( (hardware == kHardwareSfc || hardware == kHardwareSgb) &&
		data_type != kDataTypeBg)
	{
		GBGFX_LOG_ERROR("This hardware only supports the BG data type");
		return false;
	}

	switch(hardware)
	{
		case kHardwareDmg:
			s_parameters.tile_removal_max = kTileRemovalDoubles;
			s_parameters.palette_base_index = 0;
			s_parameters.palette_max_count = s_parameters.data_type == kDataTypeBg ? 1 : 2;
			s_parameters.palette_color_max_count = 4;
			s_parameters.tile_max_count = 256;
			s_parameters.bank_max_count = 1;
			s_parameters.use_transparent_color = false;
			break;
		case kHardwareCgb:
			s_parameters.tile_removal_max = kTileRemovalFlips;
			s_parameters.palette_base_index = 0;
			s_parameters.palette_max_count = 8;
			s_parameters.palette_color_max_count = 4;
			s_parameters.tile_max_count = 512;
			s_parameters.bank_max_count = 2;
			s_parameters.use_transparent_color = false;
			break;
		case kHardwareSgb:
			s_parameters.tile_removal_max = kTileRemovalDoubles;
			s_parameters.palette_base_index = 0;
			s_parameters.palette_max_count = 4;
			s_parameters.palette_color_max_count = 4;
			s_parameters.tile_max_count = 256;
			s_parameters.bank_max_count = 1;
			s_parameters.use_transparent_color = false;
			break;
		case kHardwareSfc:
			s_parameters.tile_removal_max = kTileRemovalFlips;
			s_parameters.palette_base_index = 4;
			s_parameters.palette_max_count = 3;
			s_parameters.palette_color_max_count = 16;
			s_parameters.tile_max_count = 256;
			s_parameters.bank_max_count = 1;
			s_parameters.use_transparent_color = true;
			break;
		default:
			assert(false);
			break;
	}

	return true;
}

bool isParameterValid()
{
	return s_parameters.hardware != kHardwareCount;
}

////////////////////////////////////////

Hardware getTargetHardware()
{
	return s_parameters.hardware;
}

DataType getDataType()
{
	return s_parameters.data_type;
}

bool isSprite()
{
	return getDataType() != kDataTypeBg;
}

TileRemoval getTileRemovalMax()
{
	return s_parameters.tile_removal_max;
}

uint32_t getPaletteBaseIndex()
{
	return s_parameters.palette_base_index;
}

uint32_t getPaletteMaxCount()
{
	return s_parameters.palette_max_count;
}

uint32_t getPaletteColorMaxCount()
{
	return s_parameters.palette_color_max_count;
}

uint32_t getTileMaxCount()
{
	return s_parameters.tile_max_count;
}

uint32_t getBankMaxCount()
{
	return s_parameters.bank_max_count;
}

bool getUseTransparentColor()
{
	return s_parameters.use_transparent_color;
}

////////////////////////////////////////////////////////////////////////////////

}

