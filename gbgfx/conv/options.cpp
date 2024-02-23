#include "commandline.h"
#include "options.h"

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

	const Mapping mode_mapping[] =
	{
		{ "bg", gbgfx::kModeBg },
		{ "spr8", gbgfx::kModeSprite8x8 },
		{ "spr16", gbgfx::kModeSprite8x16 },
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
		// mode
		OptionStringToInteger(
			"mode", "Specify the conversion mode", true, 'MODE', reinterpret_cast<int32_t*>(&out_options.mode),
			mode_mapping, sizeof(mode_mapping) / sizeof(mode_mapping[0])),
		// tileset
		OptionString("tileset", "The tileset image", true, 'TLS ', &out_options.tileset.image_filename),

		// tileset
		OptionStringToInteger(
			"tile-removal", "Tile removal mode", false, 'TREM', reinterpret_cast<int32_t*>(&out_options.tileset.tile_removal),
			tile_removal_mapping, sizeof(tile_removal_mapping) / sizeof(tile_removal_mapping[0])),

		// output
		OptionInteger("palette-index-offset", "Palette index offset", false, 'PALO', &out_options.output.palette_index_offset),
		OptionInteger("tile-index-offset", "Tile index offset", false, 'TILO', &out_options.output.tile_index_offset),
		OptionFlag("8800", "Use $8800 tile addressing mode", 'ADRM', &out_options.output.use_8800_addressing_mode),
		OptionFlag("use-headers", "Add headers to output files", 'HEAD', &out_options.output.add_binary_headers),
		OptionFlag("skip-export-palette", "Skip export of the palette set", 'SPAL', &out_options.output.skip_export_palette),
		OptionFlag("skip-export-tileset", "Skip export of the tileset", 'STLS', &out_options.output.skip_export_tileset),
		OptionFlag("skip-export-indices", "Skip export of the tilemaps", 'STLM', &out_options.output.skip_export_indices),
		OptionFlag("skip-export-parameter", "Skip export of the tilemaps", 'SPRM', &out_options.output.skip_export_parameters),

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
		"-hardware <hardware> -mode <mode> [options] <tileset_png> [tilemap_png...]");

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
		GBGFX_LOG_ERROR(cli_parser.getLastErrorMessage());
		return false;
	}

	return true;
}

