#pragma once

#include <vector>

#include "gfx.h"

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

struct Options
{
	struct
	{
		const char* filename = nullptr;
		std::vector<gfx::Division> divisions;
	}
	input;

	struct
	{
		const char* filename = nullptr;
	}
	output;

	bool verbose = false;
	bool help = false;
};

////////////////////////////////////////////////////////////////////////////////

bool parseCliOptions(Options& out_options, bool& out_is_help, int argc, const char** argv);

