#pragma once

#include <vector>

#include "gbgfx.h"

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

struct Options
{
	gbgfx::Hardware hardware = gbgfx::kHardwareCount;

	struct
	{
		const char* image_filename = nullptr;
		gbgfx::TileRemoval tile_removal = gbgfx::kTileRemovalNone;
		std::vector<gbgfx::Division> divisions;
	}
	tileset;

	struct
	{
		std::vector<const char*> image_filenames;
		std::vector<gbgfx::Division> divisions;
	}
	tilemap;

	struct
	{
		const char* directory = nullptr;
		int32_t palette_index_offset = 0;
		int32_t tile_index_offset = 0;
		bool use_8800_addressing_mode = false;
		bool add_binary_headers = false;
		bool skip_export_palette = false;
		bool skip_export_tileset = false;
		bool skip_export_indices = false;
		bool skip_export_parameters = false;
	}
	output;

	struct
	{
		bool generate_palette_png = false;
		bool generate_tileset_png = false;
	}
	debug;

	bool verbose = false;
	bool help = false;
};

////////////////////////////////////////////////////////////////////////////////

bool parseCliOptions(Options& out_options, bool& out_is_help, int argc, const char** argv);

