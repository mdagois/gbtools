#include <string>
#include <vector>

#include "gbgfx.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

static bool importData(
	gbgfx::Tileset& out_tileset, gbgfx::PaletteSet& out_palette_set,
	const gbgfx::Division* tileset_divisions, uint32_t tileset_division_count,
	std::vector<gbgfx::Tilemap>& out_tilemaps,
	const gbgfx::Division* tilemap_divisions, uint32_t tilemap_division_count,
	const Options& options)
{
	GBGFX_LOG_INFO("Extracting tileset and palette set from [" << options.tileset.image_filename << "]");
	if(!gbgfx::extractTileset(
		out_tileset, out_palette_set,
		tileset_divisions, tileset_division_count,
		options.tileset.tile_removal, options.tileset.image_filename))
	{
		GBGFX_LOG_ERROR("Could not extract tileset from [" << options.tileset.image_filename << "]");
		return false;
	}

	out_tilemaps.clear();
	for(auto image_filename : options.tilemap.image_filenames)
	{
		GBGFX_LOG_INFO("Extracting tilemap from [" << image_filename << "]");
		const size_t size = out_tilemaps.size();
		out_tilemaps.resize(size + 1);
		if(!gbgfx::extractTilemap(
			out_tilemaps[size], out_tileset, out_palette_set,
			tilemap_divisions, tilemap_division_count,
			image_filename))
		{
			GBGFX_LOG_ERROR("Could not extract tilemap from [" << image_filename << "]");
			return false;
		}
	}

	return true;
}

static std::string getOutputFilename(const char* input_filename, const char* output_extension)
{
	return std::string(input_filename).append(output_extension);
}

#if 0
static bool exportData(const Options& options)
{
	////////////////////////////////////////

	if(!options.output.skip_export_palette)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".pal");
		GBGFX_LOG_INFO("Exporting palette set to [" << filename << "]");
		if(!gbgfx::exportPaletteSet(
			palette_set, filename.c_str(), options.output.add_binary_headers))
		{
			GBGFX_LOG_ERROR("Could not export palette set");
			return false;
		}
	}

	if(!options.output.skip_export_tileset)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".chr");
		GBGFX_LOG_INFO("Exporting tileset to [" << filename << "]");
		if(!gbgfx::exportTileset(
			tileset, filename.c_str(), options.output.add_binary_headers))
		{
			GBGFX_LOG_ERROR("Could not export tileset");
			return false;
		}
	}

	assert(tilemaps.size() == options.tilemap.image_filenames.size()); 
	for(size_t i = 0; i < tilemaps.size(); ++i)
	{
		const char* image_filename = options.tilemap.image_filenames[i];
		const std::string idx_filename = getOutputFilename(image_filename, ".idx");
		const std::string prm_filename = getOutputFilename(image_filename, ".prm");
		const std::string atr_filename = getOutputFilename(image_filename, ".atr");
		GBGFX_LOG_INFO("Exporting tilemap to [" << idx_filename << "] and [" << prm_filename << "]");
		if(	!options.output.skip_export_tilemaps &&
			!gbgfx::exportTilemap(
				tilemaps[i],
				idx_filename.c_str(),
				options.output.skip_export_parameters ? nullptr : prm_filename.c_str(),
				options.output.skip_export_attributes ? nullptr : atr_filename.c_str(),
				options.output.add_binary_headers,
				options.output.use_8800_addressing_mode,
				options.output.palette_index_offset,
				options.output.tile_index_offset))
		{
			GBGFX_LOG_ERROR("Could not export tilemap");
			return false;
		}
	}

	////////////////////////////////////////

	if(options.debug.generate_palette_png)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".pal.png");
		GBGFX_LOG_INFO("Generating palette set PNG to [" << filename << "]");
		if(!gbgfx::writePaletteSetToPNG(filename.c_str(), palette_set))
		{
			GBGFX_LOG_ERROR("Could not write palette set PNG [" << filename << "]");
			return false;
		}
	}

	if(options.debug.generate_tileset_png)
	{
		constexpr uint32_t kTileColumnCount = 16;
		constexpr bool kClearDoubles = false;
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".chr.png");
		GBGFX_LOG_INFO("Generating tileset PNG to [" << filename << "]");
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
#endif

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

	gbgfx::setLogLevel(options.verbose ? gbgfx::kLogLevel_Info : gbgfx::kLogLevel_Error);
	if(!gbgfx::initialize(options.hardware, options.mode))
	{
		return 1;
	}

	const gbgfx::Division tileset_divisions[] =
	{
		{ 32, 32, true },
		{ 8, 16, true },
		{ 8, 8, false },
	};
	const uint32_t tileset_division_count = sizeof(tileset_divisions) / sizeof(tileset_divisions[0]);

	const gbgfx::Division tilemap_divisions[] =
	{
		{ 256, 256, true },
		{ 8, 8, true },
	};
	const uint32_t tilemap_division_count = sizeof(tilemap_divisions) / sizeof(tilemap_divisions[0]);

	gbgfx::Tileset tileset;
	gbgfx::PaletteSet palette_set;
	std::vector<gbgfx::Tilemap> tilemaps;
	if(!importData(
		tileset, palette_set, tileset_divisions, tileset_division_count,
		tilemaps, tilemap_divisions, tilemap_division_count,
		options))
	{
		return 1;
	}

	return 0;
}

