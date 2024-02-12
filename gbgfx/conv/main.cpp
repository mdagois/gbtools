#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "gbgfx.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////
// Data export
////////////////////////////////////////////////////////////////////////////////

static bool loadDataFromImages(
	gbgfx::Tileset& out_tileset, gbgfx::PaletteSet& out_palette_set,
	std::vector<gbgfx::Tilemap>& out_tilemaps,
	const Options& options)
{
	if(!gbgfx::extractTileset(
		out_tileset, out_palette_set,
		options.tileset.start_tile_row, options.tileset.tile_row_count,
		options.tileset.metatile_width, options.tileset.metatile_height,
		options.tileset.use_microtile_8x16,
		options.tileset.tile_removal != kTileRemoval_None,
		options.tileset.tile_removal == kTileRemoval_Flips,
		options.tileset.png_filename))
	{
		std::cout
			<< "Could not extract tileset from [" << options.tileset.png_filename << "]"
			<< std::endl;
		return false;
	}

	out_tilemaps.clear();
	for(auto png_filename : options.tilemap.png_filenames)
	{
		const size_t size = out_tilemaps.size();
		out_tilemaps.resize(size + 1);
		if(!gbgfx::extractTilemap(
			out_tilemaps[size], out_tileset, out_palette_set,
			options.tilemap.use_flips, png_filename))
		{
			std::cout
				<< "Could not extract tilemap from [" << png_filename << "]"
				<< std::endl;
			return false;
		}
	}

	return true;
}

static std::string getOutputFilename(const char* input_filename, const char* output_extension)
{
	return std::string(input_filename).append(output_extension);
}

static bool exportData(const Options& options)
{
	gbgfx::Tileset tileset;
	gbgfx::PaletteSet palette_set;
	std::vector<gbgfx::Tilemap> tilemaps;
	if(!loadDataFromImages(tileset, palette_set, tilemaps, options))
	{
		return false;
	}

	if(palette_set.size() > static_cast<uint32_t>(options.output.palette_max_count))
	{
		GBGFX_LOG_ERROR(
			"Too many palettes (" << palette_set.size() << " > "
			<< options.output.palette_max_count << ")");
		return false;
	}
	if(tileset.size() > static_cast<uint32_t>(options.output.tile_max_count))
	{
		GBGFX_LOG_ERROR(
			"Too many tiles (" << tileset.size() << " > "
			<< options.output.tile_max_count << ")");
		return false;
	}

	////////////////////////////////////////

	if(	!options.output.skip_export_palette &&
		!gbgfx::exportPaletteSet(
			palette_set,
			getOutputFilename(options.tileset.png_filename, ".pal").c_str(),
			options.output.add_binary_headers))
	{
		std::cout << "Could not export palette set" << std::endl;
		return false;
	}

	if(	!options.output.skip_export_tileset &&
		!gbgfx::exportTileset(
			tileset,
			getOutputFilename(options.tileset.png_filename, ".chr").c_str(),
			options.output.add_binary_headers))
	{
		std::cout << "Could not export tileset" << std::endl;
		return false;
	}

	assert(tilemaps.size() == options.tilemap.png_filenames.size()); 
	for(size_t i = 0; i < tilemaps.size(); ++i)
	{
		const char* png_filename = options.tilemap.png_filenames[i];
		if(	!options.output.skip_export_tilemaps &&
			!gbgfx::exportTilemap(
				tilemaps[i],
				getOutputFilename(png_filename, ".idx").c_str(),
				getOutputFilename(png_filename, ".prm").c_str(),
				options.output.add_binary_headers,
				options.output.palette_offset_index,
				options.output.use_8800_addressing_mode ? 128 : 0))
		{
			std::cout << "Could not export tilemap" << std::endl;
			return false;
		}
	}

	////////////////////////////////////////

#if 0
	for(uint32_t i = 0; i < gbgfx::kTileFlipType_Count; ++i)
	{
		static const char* filenames[] =
		{
			"test/demo_tileset_0.png",
			"test/demo_tileset_1.png",
			"test/demo_tileset_2.png",
			"test/demo_tileset_3.png",
		};
		static_assert(sizeof(filenames) / sizeof(filenames[0]) == gbgfx::kTileFlipType_Count);
		if(!gbgfx::writeTilesetToPNG(
			filenames[i], 16,
			tileset, static_cast<gbgfx::TileFlipType>(i),
			palette_set, i == 0))
		{
			std::cout << "Could not write tileset" << std::endl;
			return false;
		}
	}
#endif

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char** argv)
{
	Options options;
	bool is_help = 0;
	if(!parseCliOptions(options, is_help, argc, argv))
	{
		return is_help ? 0 : 1;
	}

	applyHardwareLimits(options);
	gbgfx::setLogLevel(options.verbose ? gbgfx::kLogLevel_Info : gbgfx::kLogLevel_Error);

	if(!exportData(options))
	{
		return 1;
	}

	return 0;
}

