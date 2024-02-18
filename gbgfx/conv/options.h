#pragma once

#include <algorithm>
#include <vector>

#include "gbgfx.h"

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

struct Options
{
	gbgfx::Hardware hardware = gbgfx::kHardwareCount;
	gbgfx::DataType data_type = gbgfx::kDataTypeCount;

	struct
	{
		const char* png_filename = nullptr;
		int32_t start_tile_row = 0;
		int32_t tile_row_count = gbgfx::kIterateAllRows;
		int32_t metatile_width = gbgfx::kTileSize;
		int32_t metatile_height = gbgfx::kTileSize;
		bool skip_single_color_metatiles = false;
		gbgfx::TileRemoval tile_removal = gbgfx::kTileRemovalNone;
	}
	tileset;

	struct
	{
		std::vector<const char*> png_filenames;
		bool use_flips = false;
	}
	tilemap;

	struct
	{
		int32_t palette_index_offset = 0;
		int32_t tile_index_offset = 0;
		bool use_8800_addressing_mode = false;
		bool add_binary_headers = false;

		bool skip_export_palette = false;
		bool skip_export_tileset = false;
		bool skip_export_tilemaps = false;
		bool skip_export_parameters = false;
		bool skip_export_attributes = false;
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

bool applyHardwareLimits(Options& options);
bool parseCliOptions(Options& out_options, bool& out_is_help, int argc, const char** argv);

