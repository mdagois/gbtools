#include <algorithm>
#include <cassert>
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
		GBGFX_LOG_ERROR("Could not extract tileset from [" << options.tileset.png_filename << "]");
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
			GBGFX_LOG_ERROR("Could not extract tilemap from [" << png_filename << "]");
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
		GBGFX_LOG_ERROR("Could not export palette set");
		return false;
	}

	if(	!options.output.skip_export_tileset &&
		!gbgfx::exportTileset(
			tileset,
			getOutputFilename(options.tileset.png_filename, ".chr").c_str(),
			options.output.add_binary_headers))
	{
		GBGFX_LOG_ERROR("Could not export tileset");
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
			GBGFX_LOG_ERROR("Could not export tilemap");
			return false;
		}
	}

	////////////////////////////////////////

	if(options.debug.print_palette)
	{
		//TODO
	}

	if(options.debug.print_tileset)
	{
		constexpr uint32_t kTileColumnCount = 16;
		constexpr bool kClearDoubles = false;
		const std::string filename = getOutputFilename(options.tileset.png_filename, ".chr.png");
		if(!gbgfx::writeTilesetToPNG(
			filename.c_str(), kTileColumnCount,
			tileset, gbgfx::kTileFlipType_None,
			palette_set, kClearDoubles))
		{
			GBGFX_LOG_ERROR("Could not write tileset PNG [" << filename << "]");
			return false;
		}
	}

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

