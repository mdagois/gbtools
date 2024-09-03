#include <cassert>

#include "division.h"
#include "utils/log.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Division
////////////////////////////////////////////////////////////////////////////////

static const char s_letter[] = { 'X', '|', '.', '@' };
static_assert(sizeof(s_letter) / sizeof(s_letter[0]) == kDivisionStatus_Count);

char getLetterFromStatus(DivisionStatus status)
{
	assert(status < kDivisionStatus_Count);
	return s_letter[status];
}

DivisionStatus getStatusFromLetter(char letter)
{
	for(uint32_t i = 0; i < kDivisionStatus_Count; ++i)
	{
		if(letter == s_letter[i])
		{
			return static_cast<DivisionStatus>(i);
		}
	}
	return kDivisionStatus_Invalid;
}

bool getDivisionOffset(
	uint32_t& out_offset,
	const DivisionInfo& division_info,
	uint32_t division_x, uint32_t division_y,
	uint32_t division_width, uint32_t division_height)
{
	if(division_x > division_info.image_width / division_width || division_y > division_info.image_height / division_height)
	{
		GFX_LOG_ERROR(
			"(" << division_x << ", " << division_y << ") coordinates are out of ("
			<< (division_info.image_width / division_width) << ", "
			<< (division_info.image_height / division_height) << ")");
		return false;
	}
	const DivisionStatusList* status_list = nullptr;
	for(const DivisionStatusList& list : division_info)
	{
		if(list.division.width == division_width && list.division.height == division_height)
		{
			status_list = &list;
			break;
		}
	}
	if(status_list == nullptr)
	{
		GFX_LOG_ERROR("Could not find division list of size " << division_width << "x" << division_height);
		return false;
	}

	const DivisionStatusList& last_list = division_info.back();
	const uint32_t x = division_x * (division_width / last_list.division.width);
	const uint32_t y = division_y * (division_height / last_list.division.height);
	const uint32_t index = y * last_list.division.width + x;
	out_offset = division_info.last_list_offsets[index];
	return true;
}

bool computeDivisionInfoOffsets(DivisionInfo& inout_division_info)
{
	const DivisionStatusList& last_list = inout_division_info.back();
	OffsetList& offsets = inout_division_info.last_list_offsets;
	offsets.clear();
	uint32_t offset = 0;
	for(DivisionStatus status : last_list)
	{
		offsets.push_back(offset);
		if(status == kDivisionStatus_Valid)
		{
			++offset;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

}

