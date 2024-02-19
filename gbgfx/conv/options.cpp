#include "commandline.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

bool applyHardwareLimits(Options& options)
{
	if(!gbgfx::initializeProfile(options.hardware, options.data_type))
	{
		return false;
	}

	switch(options.hardware)
	{
		case gbgfx::kHardwareDmg:
			options.output.skip_export_palette = true;
			options.output.skip_export_parameters = true;
			options.output.skip_export_attributes = true;
			break;
		case gbgfx::kHardwareCgb:
			options.output.skip_export_attributes = true;
			break;
		case gbgfx::kHardwareSgb:
			options.output.skip_export_parameters = true;
			break;
		case gbgfx::kHardwareSfc:
			options.tileset.skip_single_color_metatiles = false;
			options.output.use_8800_addressing_mode = false;
			options.output.skip_export_attributes = true;
			break;
		default:
			break;
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
		{ "dmg", gbgfx::kHardwareDmg },
		{ "cgb", gbgfx::kHardwareCgb },
		{ "sgb", gbgfx::kHardwareSgb },
		{ "sfc", gbgfx::kHardwareSfc },
	};

	const Mapping data_type_mapping[] =
	{
		{ "bg", gbgfx::kDataTypeBg },
		{ "obj8", gbgfx::kDataTypeSprite8x8 },
		{ "obj16", gbgfx::kDataTypeSprite8x16 },
	};

	const Mapping tile_removal_mapping[] =
	{
		{ "none", gbgfx::kTileRemovalNone },
		{ "doubles", gbgfx::kTileRemovalDoubles },
		{ "flips", gbgfx::kTileRemovalFlips },
	};

	Option cli_options[] =
	{
		// hardware
		OptionStringToInteger(
			"hardware", "Specify the target hardware", true, 'HARD', reinterpret_cast<int32_t*>(&out_options.hardware),
			hardware_mapping, sizeof(hardware_mapping) / sizeof(hardware_mapping[0])),
		OptionStringToInteger(
			"data-type", "Specify the data type", true, 'DATA', reinterpret_cast<int32_t*>(&out_options.data_type),
			data_type_mapping, sizeof(data_type_mapping) / sizeof(data_type_mapping[0])),

		// tileset
		OptionInteger("starting-tile-row", "Starting tile row", false, 'STRO', &out_options.tileset.start_tile_row),
		OptionInteger("tile-row-count", "Tile row count", false, 'ROWC', &out_options.tileset.tile_row_count),
		OptionInteger("tileset-metatile-width", "Tileset metatile pixel width", false, 'TSMW', &out_options.tileset.metatile_width),
		OptionInteger("tileset-metatile-height", "Tileset metatile pixel height", false, 'TSMH', &out_options.tileset.metatile_height),
		OptionFlag("skip-empty-metatile", "Skip single color metatiles", 'SSME', &out_options.tileset.skip_single_color_metatiles),
		OptionStringToInteger(
			"tile-removal", "Tile removal mode", false, 'TREM', reinterpret_cast<int32_t*>(&out_options.tileset.tile_removal),
			tile_removal_mapping, sizeof(tile_removal_mapping) / sizeof(tile_removal_mapping[0])),

		// tilemap
		OptionInteger("tilemap-metatile-width", "Tilemap metatile pixel width", false, 'TMMW', &out_options.tilemap.metatile_width),
		OptionInteger("tilemap-metatile-height", "Tilemap metatile pixel height", false, 'TMMH', &out_options.tilemap.metatile_height),

		// output
		OptionInteger("palette-index-offset", "Palette index offset", false, 'PALO', &out_options.output.palette_index_offset),
		OptionInteger("tile-index-offset", "Tile index offset", false, 'TILO', &out_options.output.tile_index_offset),
		OptionFlag("8800", "Use $8800 tile addressing mode", 'ADRM', &out_options.output.use_8800_addressing_mode),
		OptionFlag("use-headers", "Add headers to output files", 'HEAD', &out_options.output.add_binary_headers),
		OptionFlag("skip-export-palette", "Skip export of the palette set", 'SKIP', &out_options.output.skip_export_palette),
		OptionFlag("skip-export-tileset", "Skip export of the tileset", 'SKIT', &out_options.output.skip_export_tileset),
		OptionFlag("skip-export-tilemap", "Skip export of the tilemaps", 'SKIM', &out_options.output.skip_export_tilemaps),

		// debug
		OptionFlag("generate-png-palette", "Generate a PNG file of the palette", 'GENP', &out_options.debug.generate_palette_png),
		OptionFlag("generate-png-tileset", "Generate a PNG file of the tileset", 'GENT', &out_options.debug.generate_tileset_png),

		// misc
		OptionFlag("v", "Enable verbose mode", 'VERB', &out_options.verbose),
		OptionFlag("h", "Show help", 'HELP', &out_options.help),
	};

	Parser cli_parser(
		argv, argc,
		cli_options, sizeof(cli_options) / sizeof(cli_options[0]),
		"-hardware <hardware> -data-type <type> [options] <tileset_png> [tilemap_png...]");

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
				out_options.tileset.png_filename = arguments[0];
				for(int32_t i = 1; i < cli_parser.getRemainingArgumentCount(); ++i)
				{
					out_options.tilemap.png_filenames.push_back(arguments[i]);
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
		GBGFX_LOG_ERROR(cli_parser.getLastErrorMessage());
		return false;
	}

	if(out_options.tileset.png_filename == nullptr)
	{
		cli_parser.printHelp();
		out_is_help = true;
		return false;
	}

	return true;
}

