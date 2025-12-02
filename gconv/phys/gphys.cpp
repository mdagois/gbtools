#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

#include "gfx.h"
#include "import/import.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

using namespace gfx;

////////////////////////////////////////////////////////////////////////////////

static bool read(const Options options)
{
	Image image;
	if(!image.read(options.input.filename))
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

