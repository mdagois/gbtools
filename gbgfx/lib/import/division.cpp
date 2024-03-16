#include <cassert>

#include "division.h"

namespace gbgfx {

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

////////////////////////////////////////////////////////////////////////////////

}

