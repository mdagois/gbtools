#include "commandline.h"
#include "options.h"

////////////////////////////////////////////////////////////////////////////////

void applyHardwareLimits(Options& options)
{
	auto applyLimit = [](int32_t& value, int32_t maximum)
	{
		value = std::max(0, std::min(value, maximum));
	};

	switch(options.hardware)
	{
		case kHardwareDmg:
			if(options.tileset.tile_removal == kTileRemoval_Flips)
			{
				options.tileset.tile_removal = kTileRemoval_Doubles;
			}
			options.tilemap.use_flips = false;
			applyLimit(options.output.palette_max_count, options.tileset.is_sprite ? 2 : 1);
			applyLimit(options.output.tile_max_count, gbgfx::kTilesPerBank);
			options.output.skip_export_palette = true;
			break;
		case kHardwareCgb:
			applyLimit(options.output.palette_max_count, 8);
			applyLimit(options.output.tile_max_count, gbgfx::kTileMaxCount);
			break;
		default:
			break;
	}

	if(options.tileset.is_sprite)
	{
		options.output.use_8800_addressing_mode = false;
	}
}

////////////////////////////////////////////////////////////////////////////////

bool parseCliOptions(Options& out_options, bool& out_is_help, int argc, const char** argv)
{
	using namespace cli;

	out_is_help = false;

	const Mapping hardware_mapping[] =
	{
		{ "dmg", kHardwareDmg },
		{ "cgb", kHardwareCgb },
	};

	const Mapping tile_removal_mapping[] =
	{
		{ "none", kTileRemoval_None },
		{ "doubles", kTileRemoval_Doubles },
		{ "flips", kTileRemoval_Flips },
	};

	Option cli_options[] =
	{
		// hardware
		OptionStringToInteger(
			"hw", "Specify the target hardware", true, 'HARD', reinterpret_cast<int32_t*>(&out_options.hardware),
			hardware_mapping, sizeof(hardware_mapping) / sizeof(hardware_mapping[0])),

		// tileset
		OptionInteger("sr", "Starting tile row", false, 'STRO', &out_options.tileset.start_tile_row),
		OptionInteger("rc", "Tile row count", false, 'ROWC', &out_options.tileset.tile_row_count),
		OptionInteger("mw", "Metatile pixel width", false, 'METW', &out_options.tileset.metatile_width),
		OptionInteger("mh", "Metatile pixel height", false, 'METH', &out_options.tileset.metatile_height),
		OptionFlag("sp", "Treat as sprite tiles", 'SPRI', &out_options.tileset.is_sprite),
		OptionFlag("sk", "Skip single color metatiles", 'SSME', &out_options.tileset.skip_single_color_metatiles),
		OptionFlag("8x16", "Set sprite size to 8x16", '8x16', &out_options.tileset.use_microtile_8x16),
		OptionStringToInteger(
			"tr", "Tile removal mode", false, 'TREM', reinterpret_cast<int32_t*>(&out_options.tileset.tile_removal),
			tile_removal_mapping, sizeof(tile_removal_mapping) / sizeof(tile_removal_mapping[0])),

		// tilemap
		OptionFlag("fl", "Use flips when exporting tilemaps", 'FLIP', &out_options.tilemap.use_flips),

		// output
		OptionInteger("po", "Palette index offset", false, 'PALO', &out_options.output.palette_offset_index),
		OptionInteger("pc", "Palette max count", false, 'PAMC', &out_options.output.palette_max_count),
		OptionInteger("tc", "Tile max count", false, 'TILC', &out_options.output.tile_max_count),
		OptionFlag("8800", "Use $8800 tile addressing mode", 'ADRM', &out_options.output.use_8800_addressing_mode),
		OptionFlag("bh", "Add headers to output files", 'HEAD', &out_options.output.add_binary_headers),
		OptionFlag("ps", "Skip export of the palette set", 'SKIP', &out_options.output.skip_export_palette),
		OptionFlag("ts", "Skip export of the tileset", 'SKIT', &out_options.output.skip_export_tileset),
		OptionFlag("ms", "Skip export of the tilemaps", 'SKIM', &out_options.output.skip_export_tilemaps),

		// debug
		OptionFlag("gp", "Generate a PNG file of the palette", 'GENP', &out_options.debug.generate_palette_png),
		OptionFlag("gt", "Generate a PNG file of the tileset", 'GENT', &out_options.debug.generate_tileset_png),

		// misc
		OptionFlag("v", "Enable verbose mode", 'VERB', &out_options.verbose),
		OptionFlag("h", "Show help", 'HELP', &out_options.help),
	};

	Parser cli_parser(
		argv, argc,
		cli_options, sizeof(cli_options) / sizeof(cli_options[0]),
		"-hw <hardware> [options] <tileset_png> [tilemap_png...]");

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

