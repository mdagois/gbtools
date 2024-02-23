#include <cassert>
#include <string>
#include <vector>

#include "gbgfx.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

static bool importData(
	gbgfx::Tileset& out_tileset, gbgfx::PaletteSet& out_palette_set,
	std::vector<gbgfx::Tilemap>& out_tilemaps,
	const Options& options)
{
	////////////////////////////////////////
	const gbgfx::Division tileset_divisions[] =
	{
		//{ 32, 32, true },
		//{ 8, 16, true },
		{ 8, 8, false },
	};
	const uint32_t tileset_division_count = sizeof(tileset_divisions) / sizeof(tileset_divisions[0]);

	const gbgfx::Division tilemap_divisions[] =
	{
		//{ 256, 256, true },
		{ 8, 8, true },
	};
	const uint32_t tilemap_division_count = sizeof(tilemap_divisions) / sizeof(tilemap_divisions[0]);
	////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

static std::string getOutputFilename(const char* input_filename, const char* output_extension)
{
	return std::string(input_filename).append(output_extension);
}

////////////////////////////////////////////////////////////////////////////////

static bool exportData(
	const gbgfx::Tileset& tileset, const gbgfx::PaletteSet& palette_set,
	const std::vector<gbgfx::Tilemap>& tilemaps, const Options& options)
{
	if(!options.output.skip_export_palette)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".pal");
		GBGFX_LOG_INFO("Exporting palette set to [" << filename << "]");
		if(!gbgfx::exportPaletteSet(
			palette_set, options.output.add_binary_headers, filename.c_str()))
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
			tileset, options.output.add_binary_headers, filename.c_str()))
		{
			GBGFX_LOG_ERROR("Could not export tileset");
			return false;
		}
	}

	if(!options.output.skip_export_indices || !options.output.skip_export_parameters)
	{
		assert(tilemaps.size() == options.tilemap.image_filenames.size());
		for(size_t i = 0; i < tilemaps.size(); ++i)
		{
			const char* image_filename = options.tilemap.image_filenames[i];
			const std::string idx_filename = getOutputFilename(image_filename, ".idx");
			const std::string prm_filename = getOutputFilename(image_filename, ".prm");
			GBGFX_LOG_INFO("Exporting tilemap to [" << idx_filename << "] and [" << prm_filename << "]");
			if(!gbgfx::exportTilemap(
				tilemaps[i],
				options.output.add_binary_headers, options.output.use_8800_addressing_mode,
				options.output.palette_index_offset, options.output.tile_index_offset,
				options.output.skip_export_indices ? nullptr : idx_filename.c_str(),
				options.output.skip_export_parameters ? nullptr : prm_filename.c_str()))
			{
				GBGFX_LOG_ERROR("Could not export tilemap");
				return false;
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

static bool outputDebugData(
	const gbgfx::Tileset& tileset, const gbgfx::PaletteSet& palette_set, const Options& options)
{
	if(options.debug.generate_palette_png)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".pal.png");
		GBGFX_LOG_INFO("Generating palette set PNG to [" << filename << "]");
		if(!gbgfx::writePaletteSetToPNG(palette_set, filename.c_str()))
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
			tileset, gbgfx::kTileFlipType_None, palette_set,
			kTileColumnCount, kClearDoubles, filename.c_str()))
		{
			GBGFX_LOG_ERROR("Could not write tileset PNG [" << filename << "]");
			return false;
		}
	}

	return true;
}

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

	gbgfx::Tileset tileset;
	gbgfx::PaletteSet palette_set;
	std::vector<gbgfx::Tilemap> tilemaps;

	if(!importData(tileset, palette_set, tilemaps, options))
	{
		return 1;
	}

	if(!exportData(tileset, palette_set, tilemaps, options))
	{
		return 1;
	}

	if(!outputDebugData(tileset, palette_set, options))
	{
		return 1;
	}

	return 0;
}

