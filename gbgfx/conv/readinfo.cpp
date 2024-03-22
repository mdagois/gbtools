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

	return 0;
}

