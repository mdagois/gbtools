#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "gfx.h"
#include "import/import.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

using namespace gfx;

////////////////////////////////////////////////////////////////////////////////

static bool read(Options options)
{
	Image image;
	if(!image.read(options.input.filename))
	{
		return false;
	}

	if(options.input.divisions.empty())
	{
		options.input.divisions.push_back({ 8, image.getHeight(), false });
		options.input.divisions.push_back({ 8, 8, true });
	}

	DivisionInfo division_info;
	Rectangle rectangle;
	if(!image.iterateTiles(
		division_info,
		options.input.divisions.data(),
		static_cast<uint32_t>(options.input.divisions.size()),
		rectangle,
		[](const ImageTile&, uint32_t x, uint32_t y)
		{
			std::cout << x << ", " << y << std::endl;
			return true;
		}))
	{
		return false;
	}

	return true;
}

int main(int argc, const char** argv)
{
	Options options;
	bool is_help = 0;
	if(!parseCliOptions(options, is_help, argc, argv))
	{
		return is_help ? 0 : 1;
	}

	gfx::setLogLevel(options.verbose ? gfx::kLogLevel_Info : gfx::kLogLevel_Error);

	if(!read(options))
	{
		return 1;
	}

	return 0;
}

