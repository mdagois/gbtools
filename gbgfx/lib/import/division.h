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

struct DivisionInfo : std::vector<DivisionStatusList>
{
	uint32_t image_width;
	uint32_t image_height;
};

char getLetterFromStatus(DivisionStatus status);
DivisionStatus getStatusFromLetter(char letter);

////////////////////////////////////////////////////////////////////////////////

}

