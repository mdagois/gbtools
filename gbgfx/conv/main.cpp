#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

#include "gbgfx.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

static bool importData(
	gbgfx::Tileset& out_tileset, uint32_t& out_tileset_column_count,
	gbgfx::PaletteSet& out_palette_set, std::vector<gbgfx::Tilemap>& out_tilemaps,
	gbgfx::DivisionInfo& out_tileset_division_info, std::vector<gbgfx::DivisionInfo>& out_tilemap_division_infos,
	const Options& options)
{
	GBGFX_LOG_INFO("Extracting tileset and palette set from [" << options.tileset.image_filename << "]");
	gbgfx::Image tileset_image;
	if(!tileset_image.read(options.tileset.image_filename))
	{
		return false;
	}
	if(!gbgfx::extractTileset(
		out_tileset, out_palette_set,  out_tileset_division_info,
		options.tileset.divisions, options.tileset.tile_removal, tileset_image))
	{
		GBGFX_LOG_ERROR("Could not extract tileset from [" << tileset_image.getFilename() << "]");
		return false;
	}
	out_tileset_column_count = tileset_image.getWidth() / gbgfx::getBasicTileWidth();

	out_tilemaps.clear();
	out_tilemap_division_infos.clear();
	for(auto image_filename : options.tilemap.image_filenames)
	{
		GBGFX_LOG_INFO("Extracting tilemap from [" << image_filename << "]");
		const size_t size = out_tilemaps.size();
		out_tilemaps.resize(size + 1);
		out_tilemap_division_infos.resize(size + 1);
		if(!gbgfx::extractTilemap(
			out_tilemaps[size], out_tilemap_division_infos[size],
			out_tileset, out_palette_set,
			options.tilemap.divisions, image_filename))
		{
			GBGFX_LOG_ERROR("Could not extract tilemap from [" << image_filename << "]");
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

static const std::string getOutputFilename(
	const char* input_filename, const char* output_extension,
	const Options& options)
{
	std::filesystem::path input_filepath(input_filename);
	if(options.output.directory != nullptr)
	{
		std::filesystem::path output_directory = std::filesystem::path(options.output.directory);
		output_directory /= input_filepath.stem();
		input_filepath = output_directory;
	}
	return input_filepath.replace_extension(output_extension).u8string();
}

////////////////////////////////////////////////////////////////////////////////

static bool exportData(
	const gbgfx::Tileset& tileset, const gbgfx::PaletteSet& palette_set,
	const std::vector<gbgfx::Tilemap>& tilemaps, const Options& options)
{
	if(!options.output.skip_export_palette)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".pal", options);
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
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".chr", options);
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
			const std::string idx_filename = getOutputFilename(image_filename, ".idx", options);
			const std::string prm_filename = getOutputFilename(image_filename, ".prm", options);
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

static bool exportInfo(const gbgfx::DivisionInfo tileset_division_info, const Options& options)
{
	const std::string filename = getOutputFilename(options.tileset.image_filename, ".info", options);
	GBGFX_LOG_INFO("Writing tileset info to [" << filename << "]");
	if(!gbgfx::writeDivisionInfo(tileset_division_info, filename.c_str()))
	{
		GBGFX_LOG_ERROR("Could not write the tileset division info [" << filename << "]");
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

static bool outputDebugData(
	const gbgfx::Tileset& tileset, const gbgfx::PaletteSet& palette_set,
	uint32_t tileset_column_count, const Options& options)
{
	if(options.debug.generate_palette_png)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".pal.png", options);
		GBGFX_LOG_INFO("Generating palette set PNG to [" << filename << "]");
		if(!gbgfx::writePaletteSetToPNG(palette_set, filename.c_str()))
		{
			GBGFX_LOG_ERROR("Could not write palette set PNG [" << filename << "]");
			return false;
		}
	}

	if(options.debug.generate_tileset_png)
	{
		constexpr bool kClearDoubles = false;
		const std::string filename = getOutputFilename(options.tileset.image_filename, ".chr.png", options);
		GBGFX_LOG_INFO("Generating tileset PNG to [" << filename << "]");
		if(!gbgfx::writeTilesetToPNG(
			tileset, gbgfx::kTileFlipType_None, palette_set,
			tileset_column_count, kClearDoubles, filename.c_str()))
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
	if(!gbgfx::initialize(options.hardware))
	{
		return 1;
	}

	gbgfx::Tileset tileset;
	uint32_t tileset_column_count = 0;
	gbgfx::PaletteSet palette_set;
	std::vector<gbgfx::Tilemap> tilemaps;
	gbgfx::DivisionInfo tileset_division_info;
	std::vector<gbgfx::DivisionInfo> tilemap_division_infos;

	if(!importData(
		tileset, tileset_column_count, palette_set, tilemaps,
		tileset_division_info, tilemap_division_infos,
		options))
	{
		return 1;
	}

	if(!exportData(tileset, palette_set, tilemaps, options))
	{
		return 1;
	}

	if(!exportInfo(tileset_division_info, options))
	{
		return 1;
	}

	if(!outputDebugData(tileset, palette_set, tileset_column_count, options))
	{
		return 1;
	}

#if 0
	{
		gbgfx::DivisionInfo info;
		assert(loadDivisionInfo(info, "output/unnamed_ff_spritesheet.info"));
		printDivisionInfo(info);
	}
#endif

	return 0;
}

