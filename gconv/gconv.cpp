#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

#include "gfx.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

static bool importPaletteSet(gfx::PaletteSet& out_palette_set, const Options& options)
{
	if(options.tileset.palette_set_filename == nullptr)
	{
		return true;
	}

	gfx::Image image;
	if(!image.read(options.tileset.palette_set_filename))
	{
		return false;
	}

	const gfx::ColorRGBA* pixels = image.getPixels();
	for(uint32_t i = 0; i < image.getHeight(); ++i)
	{
		out_palette_set.loadRawPaletteData(pixels, image.getWidth());
		pixels += image.getWidth();
	}

	out_palette_set.lock();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

static bool importData(
	gfx::Tileset& out_tileset, uint32_t& out_tileset_column_count,
	gfx::PaletteSet& out_palette_set, std::vector<gfx::Tilemap>& out_tilemaps,
	gfx::DivisionInfo& out_tileset_division_info, std::vector<gfx::DivisionInfo>& out_tilemap_division_infos,
	const Options& options)
{
	uint32_t largest_width = 0;
	for(auto image_filename : options.tileset.image_filenames)
	{
		GFX_LOG_INFO("Extracting tileset and palette set from [" << image_filename << "]");
		gfx::Image tileset_image;
		if(!tileset_image.read(image_filename))
		{
			return false;
		}
		if(!gfx::extractTileset(
			out_tileset, out_palette_set,  out_tileset_division_info,
			options.tileset.divisions, options.tileset.tile_removal, tileset_image))
		{
			GFX_LOG_ERROR("Could not extract tileset from [" << tileset_image.getFilename() << "]");
			return false;
		}
		if(largest_width < tileset_image.getWidth())
		{
			largest_width = tileset_image.getWidth();
		}
	}
	out_tileset_column_count = largest_width / gfx::getBasicTileWidth();

	out_tilemaps.clear();
	out_tilemap_division_infos.clear();
	for(auto image_filename : options.tilemap.image_filenames)
	{
		GFX_LOG_INFO("Extracting tilemap from [" << image_filename << "]");
		const size_t size = out_tilemaps.size();
		out_tilemaps.resize(size + 1);
		out_tilemap_division_infos.resize(size + 1);
		if(!gfx::extractTilemap(
			out_tilemaps[size], out_tilemap_division_infos[size],
			out_tileset, out_palette_set,
			options.tilemap.divisions, image_filename))
		{
			GFX_LOG_ERROR("Could not extract tilemap from [" << image_filename << "]");
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
	const gfx::Tileset& tileset, const gfx::PaletteSet& palette_set,
	const std::vector<gfx::Tilemap>& tilemaps, const Options& options)
{
	if(!options.output.skip_export_palette)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filenames[0], ".pal", options);
		GFX_LOG_INFO("Exporting palette set to [" << filename << "]");
		if(!gfx::exportPaletteSet(
			palette_set, options.output.add_binary_headers, filename.c_str()))
		{
			GFX_LOG_ERROR("Could not export palette set");
			return false;
		}
	}

	if(!options.output.skip_export_tileset)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filenames[0], ".chr", options);
		GFX_LOG_INFO("Exporting tileset to [" << filename << "]");
		if(!gfx::exportTileset(
			tileset, options.output.add_binary_headers, filename.c_str()))
		{
			GFX_LOG_ERROR("Could not export tileset");
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
			GFX_LOG_INFO("Exporting tilemap to [" << idx_filename << "] and [" << prm_filename << "]");
			if(!gfx::exportTilemap(
				tilemaps[i],
				options.output.add_binary_headers, options.output.use_8800_addressing_mode,
				options.output.palette_index_offset, options.output.tile_index_offset,
				options.output.skip_export_indices ? nullptr : idx_filename.c_str(),
				options.output.skip_export_parameters ? nullptr : prm_filename.c_str()))
			{
				GFX_LOG_ERROR("Could not export tilemap");
				return false;
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

static bool exportInfo(
	const gfx::DivisionInfo tileset_division_info,
	const std::vector<gfx::DivisionInfo>& tilemap_division_infos,
	const Options& options)
{
	if(!options.output.skip_export_tileset_info)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filenames[0], ".chr.info", options);
		GFX_LOG_INFO("Writing tileset info to [" << filename << "]");
		if(!gfx::writeDivisionInfo(tileset_division_info, filename.c_str()))
		{
			GFX_LOG_ERROR("Could not write the tileset division info [" << filename << "]");
			return false;
		}
	}

	if(!options.output.skip_export_tilemap_info)
	{
		assert(tilemap_division_infos.size() == options.tilemap.image_filenames.size());
		for(size_t i = 0; i < tilemap_division_infos.size(); ++i)
		{
			const std::string filename = getOutputFilename(options.tilemap.image_filenames[i], ".tlm.info", options);
			GFX_LOG_INFO("Writing tilemap info to [" << filename << "]");
			if(!gfx::writeDivisionInfo(tilemap_division_infos[i], filename.c_str()))
			{
				GFX_LOG_ERROR("Could not write the tilemap division info [" << filename << "]");
				return false;
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

static bool outputDebugData(
	const gfx::Tileset& tileset, const gfx::PaletteSet& palette_set,
	uint32_t tileset_column_count, const Options& options)
{
	if(options.debug.generate_palette_png)
	{
		const std::string filename = getOutputFilename(options.tileset.image_filenames[0], ".pal.png", options);
		GFX_LOG_INFO("Generating palette set PNG to [" << filename << "]");
		if(!gfx::writePaletteSetToPNG(palette_set, filename.c_str()))
		{
			GFX_LOG_ERROR("Could not write palette set PNG [" << filename << "]");
			return false;
		}
	}

	if(options.debug.generate_tileset_png)
	{
		constexpr bool kClearDoubles = false;
		const std::string filename = getOutputFilename(options.tileset.image_filenames[0], ".chr.png", options);
		GFX_LOG_INFO("Generating tileset PNG to [" << filename << "]");
		if(!gfx::writeTilesetToPNG(
			tileset, gfx::kTileFlipType_None, palette_set,
			tileset_column_count, kClearDoubles, filename.c_str()))
		{
			GFX_LOG_ERROR("Could not write tileset PNG [" << filename << "]");
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

	gfx::setLogLevel(options.verbose ? gfx::kLogLevel_Info : gfx::kLogLevel_Error);
	if(!gfx::initialize(options.hardware))
	{
		return 1;
	}

	gfx::Tileset tileset;
	uint32_t tileset_column_count = 0;
	gfx::PaletteSet palette_set;
	std::vector<gfx::Tilemap> tilemaps;
	gfx::DivisionInfo tileset_division_info;
	std::vector<gfx::DivisionInfo> tilemap_division_infos;

	if(!importPaletteSet(palette_set, options))
	{
		return 1;
	}

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

	if(!exportInfo(tileset_division_info, tilemap_division_infos, options))
	{
		return 1;
	}

	if(!outputDebugData(tileset, palette_set, tileset_column_count, options))
	{
		return 1;
	}

	return 0;
}

