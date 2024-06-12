#include <regex>
#include <string>

#include "commandline.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

static bool parseDivisions(std::vector<gfx::Division>& out_divisions, std::string sequence)
{
	std::regex pattern("(\\d+)x(\\d+)([ks])");
	auto match_begin = std::sregex_iterator(sequence.begin(), sequence.end(), pattern);
	auto match_end = std::sregex_iterator();
	for(std::sregex_iterator it = match_begin; it != match_end; ++it)
	{
		std::smatch match = *it;
		if(match.size() != 4)
		{
			GFX_LOG_ERROR("Unspected error when matching the division sequence");
			return false;
		}

		gfx::Division division;
		division.width = std::stoi(match[1]);
		division.height = std::stoi(match[2]);
		division.skip_transparent = (match[3].str())[0] == 's';
		out_divisions.push_back(division);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool parseCliOptions(Options& out_options, bool& out_is_help, int argc, const char** argv)
{
	using namespace cli;

	out_is_help = false;

	const Mapping hardware_mapping[] =
	{
		{ "dmg-bg", "DMG background", gfx::kHardware_Dmg_Background },
		{ "dmg-sp", "DMG sprites", gfx::kHardware_Dmg_Sprite },
		{ "cgb-bg", "CGB background", gfx::kHardware_Cgb_Background },
		{ "cgb-sp", "CGB sprites", gfx::kHardware_Cgb_Sprite },
		{ "sgb-bg", "SGB background", gfx::kHardware_Sgb_Background },
		{ "sgb-border", "SGB border", gfx::kHardware_Sgb_Border },
		{ "printer", "Pocket Printer", gfx::kHardware_Printer },
	};

	const Mapping tile_removal_mapping[] =
	{
		{ "none", "no tiles are removed", gfx::kTileRemovalNone },
		{ "doubles", "double tiles are removed", gfx::kTileRemovalDoubles },
		{ "flips", "tile flips are removed", gfx::kTileRemovalFlips },
	};

	const char* tileset_divisions = nullptr;
	const char* tilemap_divisions = nullptr;

	enum : uint32_t
	{
		kOptionHard,
		kOptionTileset,
		kOptionTilesetDivisions,
		kOptionTileRemoval,
		kOptionInputPaletteSet,
		kOptionTilemapDivisions,
		kOptionOutputDirectory,
		kOptionPaletteIndexOffset,
		kOptionTileIndexOffset,
		kOption8800Addressing,
		kOptionAddHeaders,
		kOptionSkipExportPalette,
		kOptionSkipExportTileset,
		kOptionSkipExportIndices,
		kOptionSkipExportParameter,
		kOptionSkipExportTilesetInfo,
		kOptionSkipExportTilemapInfo,
		kOptionGeneratePalette,
		kOptionGenerateTileset,
		kOptionVerbose,
		kOptionHelp,
	};

	Option cli_options[] =
	{
		// hardware
		OptionStringToInteger(
			"hardware", "hw", "The target hardware", true, kOptionHard, reinterpret_cast<int32_t*>(&out_options.hardware),
			hardware_mapping, sizeof(hardware_mapping) / sizeof(hardware_mapping[0])),
		// tileset
		OptionString("tileset", "ts", "The tileset image", true, kOptionTileset, &out_options.tileset.image_filename),
		OptionString("tileset-divisions", "tsd", "The tileset division", false, kOptionTilesetDivisions, &tileset_divisions),
		OptionStringToInteger(
			"tile-removal", "trm", "Tile removal mode", false, kOptionTileRemoval, reinterpret_cast<int32_t*>(&out_options.tileset.tile_removal),
			tile_removal_mapping, sizeof(tile_removal_mapping) / sizeof(tile_removal_mapping[0])),
		OptionString("input-palette-set", "ips", "Provided palette set image", false, kOptionInputPaletteSet, &out_options.tileset.palette_set_filename),
		// tilemap
		OptionString("tilemap-divisions", "tmd", "The tilemap division", false, kOptionTilemapDivisions, &tilemap_divisions),
		// output
		OptionString("output-directory", "o", "The output directory", false, kOptionOutputDirectory, &out_options.output.directory),
		OptionInteger("palette-index-offset", "pio", "Palette index offset", false, kOptionPaletteIndexOffset, &out_options.output.palette_index_offset),
		OptionInteger("tile-index-offset", "tio", "Tile index offset", false, kOptionTileIndexOffset, &out_options.output.tile_index_offset),
		OptionFlag("8800-addressing", "8800", "Use $8800 tile addressing mode", kOption8800Addressing, &out_options.output.use_8800_addressing_mode),
		OptionFlag("use-headers", "hd", "Add headers to output files", kOptionAddHeaders, &out_options.output.add_binary_headers),
		OptionFlag("skip-export-palette", "spal", "Skip export of the palette set", kOptionSkipExportPalette, &out_options.output.skip_export_palette),
		OptionFlag("skip-export-tileset", "stls", "Skip export of the tileset", kOptionSkipExportTileset, &out_options.output.skip_export_tileset),
		OptionFlag("skip-export-indices", "sidx", "Skip export of the tilemap indices", kOptionSkipExportIndices, &out_options.output.skip_export_indices),
		OptionFlag("skip-export-parameter", "sprm", "Skip export of the tilemap parameters", kOptionSkipExportParameter, &out_options.output.skip_export_parameters),
		OptionFlag("skip-export-tileset-info", "sti", "Skip export of the tileset info", kOptionSkipExportTilesetInfo, &out_options.output.skip_export_tileset_info),
		OptionFlag("skip-export-tilemap-info", "smi", "Skip export of the tilemap info", kOptionSkipExportTilemapInfo, &out_options.output.skip_export_tilemap_info),
		// debug
		OptionFlag("generate-png-palette", "gpal", "Generate a PNG file of the palette", kOptionGeneratePalette, &out_options.debug.generate_palette_png),
		OptionFlag("generate-png-tileset", "gtls", "Generate a PNG file of the tileset", kOptionGenerateTileset, &out_options.debug.generate_tileset_png),
		// misc
		OptionFlag("verbose", "v", "Enable verbose mode", kOptionVerbose, &out_options.verbose),
		OptionFlag("help", "h", "Show help", kOptionHelp, &out_options.help),
	};

	Parser cli_parser(
		argv, argc,
		cli_options, sizeof(cli_options) / sizeof(cli_options[0]),
		"--hardware <hardware> --tileset <tileset_png> [options] [tilemap_png...]");

	uint32_t code;
	const char* parameter;
	Error error;
	while(cli_parser.getNextOption(code, parameter, error))
	{
		switch(code)
		{
			case kRemainingCode:
			{
				const char** arguments = cli_parser.getRemainingArguments();
				for(int32_t i = 0; i < cli_parser.getRemainingArgumentCount(); ++i)
				{
					out_options.tilemap.image_filenames.push_back(arguments[i]);
				}
			}
			default:
			{
				break;
			}
		}
	}

	if(out_options.help)
	{
		cli_parser.printHelp();
		out_is_help = true;
		return false;
	}

	if(error != Error::kNone)
	{
		GFX_LOG_ERROR(cli_parser.getLastErrorMessage());
		return false;
	}

	if( tileset_divisions != nullptr &&
		!parseDivisions(out_options.tileset.divisions, tileset_divisions))
	{
		GFX_LOG_ERROR("Cannot parse tileset divisions");
		return false;
	}
	if( tilemap_divisions != nullptr &&
		!parseDivisions(out_options.tilemap.divisions, tilemap_divisions))
	{
		GFX_LOG_ERROR("Cannot parse tilemap divisions");
		return false;
	}

	return true;
}

