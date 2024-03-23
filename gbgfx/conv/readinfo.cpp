#include <cassert>
#include <iostream>

#include "gbgfx.h"

////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char** argv)
{
	if(argc < 2)
	{
		std::cout << "Input filename not specified" << std::endl;
		return 1;
	}

	const char* filename = argv[1];
	gbgfx::DivisionInfo division_info;
	if(!gbgfx::loadDivisionInfo(division_info, filename))
	{
		std::cout << "Could not load file [" << filename << "]" << std::endl;
		return 1;
	}

	if(!gbgfx::printDivisionInfo(division_info))
	{
		std::cout << "Could not print division info from file [" << filename << "]" << std::endl;
		return 1;
	}

#if 0
	struct
	{
		uint32_t x;
		uint32_t y;
	}
	coords[] =
	{
		{  0,  0 },
		{ 14,  3 },
		{  0,  8 },
		{  1,  8 },
		{  2,  8 },
		{  3,  8 },
	};
	for(uint32_t i = 0; i < sizeof(coords) / sizeof(coords[0]); ++i)
	{
		uint32_t offset;
		if(getDivisionOffset(offset, division_info, coords[i].x, coords[i].y, 16, 16))
		{
			std::cout << "offset[" << i << "]=" << offset << std::endl;
		}
		else
		{
			std::cout << "Cannot get offset " << i << std::endl;
			return 1;
		}
	}
#endif
	return 0;
}

