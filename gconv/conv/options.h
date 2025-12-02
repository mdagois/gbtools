#pragma once

#include <vector>

#include "gfx.h"

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

struct Options
{
	gfx::Hardware hardware = gfx::kHardwareCount;

	struct
	{
		const char* image_filename = nullptr;
		gfx::TileRemoval tile_removal = gfx::kTileRemovalNone;
		std::vector<gfx::Division> divisions;
		gfx::Rectangle rectangle;
		const char* palette_set_filename = nullptr;
	}
	tileset;

	struct TilemapEntry
	{
		std::string image_filename;
		std::string image_disguise_filename;
		std::vector<gfx::Division> divisions;
		gfx::Rectangle rectangle;
	};

	struct
	{
		std::vector<TilemapEntry> entries;
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
		bool skip_export_tileset_info = false;
		bool skip_export_tilemap_info = false;
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

