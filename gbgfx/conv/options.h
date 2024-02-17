#pragma once

#include <algorithm>
#include <vector>

#include "gbgfx.h"

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

enum Hardware : int32_t
{
	kHardwareDmg,
	kHardwareCgb,
	kHardwareSgb,
	kHardwareSfc,
};

enum TileRemoval : int32_t
{
	kTileRemoval_None,
	kTileRemoval_Doubles,
	kTileRemoval_Flips,
};

////////////////////////////////////////////////////////////////////////////////

struct Options
{
	Hardware hardware = kHardwareDmg;

	struct
	{
		const char* png_filename = nullptr;
		int32_t start_tile_row = 0;
		int32_t tile_row_count = gbgfx::kIterateAllRows;
		int32_t metatile_width = gbgfx::kTileSize;
		int32_t metatile_height = gbgfx::kTileSize;
		bool is_sprite = false;
		bool skip_single_color_metatiles = false;
		bool use_microtile_8x16 = false;
		TileRemoval tile_removal = kTileRemoval_None;
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
		int32_t palette_offset_index = 0;
		int32_t palette_max_count = gbgfx::kPaletteMaxCount;
		int32_t tile_max_count = gbgfx::kTileMaxCount;
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

void applyHardwareLimits(Options& options);
bool parseCliOptions(Options& out_options, bool& out_is_help, int argc, const char** argv);

