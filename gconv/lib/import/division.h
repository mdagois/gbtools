#pragma once

#include <cstdint>
#include <vector>

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Division
////////////////////////////////////////////////////////////////////////////////

struct Division
{
	uint32_t width;
	uint32_t height;
	bool skip_transparent;
};

enum DivisionStatus : uint8_t
{
	kDivisionStatus_Valid,
	kDivisionStatus_Transparent,
	kDivisionStatus_Skipped,
	kDivisionStatus_Invalid,
	kDivisionStatus_Count,
};

struct DivisionStatusList : public std::vector<DivisionStatus>
{
	Division division;
};

typedef std::vector<uint32_t> OffsetList;

struct DivisionInfo : std::vector<DivisionStatusList>
{
	uint32_t image_width;
	uint32_t image_height;

	OffsetList last_list_offsets;
};

char getLetterFromStatus(DivisionStatus status);
DivisionStatus getStatusFromLetter(char letter);
bool getDivisionOffset(
	uint32_t& out_offset,
	const DivisionInfo& division_info,
	uint32_t division_x, uint32_t division_y,
	uint32_t division_width, uint32_t division_height);
bool computeDivisionInfoOffsets(DivisionInfo& inout_division_info);

////////////////////////////////////////////////////////////////////////////////

}

