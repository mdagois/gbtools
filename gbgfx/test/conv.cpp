#include <cassert>
#include <iostream>
#include <vector>

#include "commandline.h"
#include "gbgfx.h"

/*
TODO

struct
{
	uint32_t palette_offset_index;
	uint32_t verbosity_level;
	uint32_t palette_max_count;
	bool output_palette;
	bool output_tilemap_parameters;
	bool remove_double_tiles;
	bool use_tile_flips;
	bool use_tile_bank;
	bool use_8800_addressing_mode;
};

DMG
	palette_offset_index		0
	verbosity_level				1
	palette_max_count			1|2
	output_palette				must support
	output_tilemap_parameters	n/a
	remove_double_tiles			supported
	use_tile_flips				n/a
	use_tile_bank				n/a
	use_8800_addressing_mode	supported
	
CGB
	palette_offset_index		0
	verbosity_level				1
	palette_max_count			8
	output_palette				supported
	output_tilemap_parameters	supported
	remove_double_tiles			supported
	use_tile_flips				supported
	use_tile_bank				supported
	use_8800_addressing_mode	supported
*/

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

enum Mode : int32_t
{
	kModeDmg,
	kModeCgb,
};

struct Options
{
	Options();

	Mode mode;
	bool verbose;
	bool help;

	const char* tileset_filename;
	std::vector<const char*> tilemap_filenames;
};

Options::Options()
: mode(kModeDmg)
, verbose(false)
, help(false)
, tileset_filename(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////
// CLI
////////////////////////////////////////////////////////////////////////////////

bool parseCliOptions(Options& out_options, int& out_ret, int argc, const char** argv)
{
	using namespace cli;

	out_ret = 0;

	const Mapping mode_mapping[] =
	{
		{ "dmg", kModeDmg },
		{ "cgb", kModeCgb },
	};

	Option cli_options[] =
	{
		OptionStringToInteger(
			"m", "Set conversion mode", true, 'MODE', reinterpret_cast<int32_t*>(&out_options.mode),
			mode_mapping, sizeof(mode_mapping) / sizeof(mode_mapping[0])),
		OptionFlag("v", "Enable verbose mode", 'VERB', &out_options.verbose),
		OptionFlag("h", "Show help", 'HELP', &out_options.help),
	};

	Parser cli_parser(
		argv, argc,
		cli_options, sizeof(cli_options) / sizeof(cli_options[0]),
		"-m <mode> [options] <tileset_png> [tilemap_png...]");

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
				out_options.tileset_filename = arguments[0];
				for(int32_t i = 1; i < cli_parser.getRemainingArgumentCount(); ++i)
				{
					out_options.tilemap_filenames.push_back(arguments[i]);
				}
			}
			default:
			{
				break;
			}
		}
	}

	if(out_options.tileset_filename == nullptr || out_options.help)
	{
		cli_parser.printHelp();
		return false;
	}

	if(error != Error::kNone)
	{
		std::cout << cli_parser.getLastErrorMessage() << std::endl;
		out_ret = 1;
		return false;
	}

	gbgfx::setLogLevel(out_options.verbose ? gbgfx::kLogLevel_Info : gbgfx::kLogLevel_Error);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char** argv)
{
	Options options;
	int ret = 0;
	if(!parseCliOptions(options, ret, argc, argv))
	{
		return ret;
	}

	////////////////////////////////////////

	gbgfx::Tileset tileset;
	gbgfx::PaletteSet palette_set;
	if(!gbgfx::extractTileset(
		tileset, palette_set,
		8, gbgfx::kIterateAllRows, gbgfx::kTileSize, gbgfx::kTileSize,
		false, false, false,
		"test/demo.png"))
	{
		std::cout << "Could not extract tileset" << std::endl;
		return 1;
	}

	gbgfx::Tilemap tilemap;
	if(!gbgfx::extractTilemap(tilemap, tileset, palette_set, false, "test/demo_tlm.png"))
	{
		std::cout << "Could not extract tilemap" << std::endl;
		return 1;
	}

	////////////////////////////////////////

	const bool use_headers = false;

	if(!gbgfx::exportPaletteSet(palette_set, "test/palette.pal", use_headers))
	{
		std::cout << "Could not export palette set" << std::endl;
		return 1;
	}

	if(!gbgfx::exportTileset(tileset, "test/tileset.chr", use_headers))
	{
		std::cout << "Could not export tileset" << std::endl;
		return 1;
	}

	if(!gbgfx::exportTilemap(
		tilemap, "test/tilemap.idx", "test/tilemap.prm",
		use_headers, 0, 128))
	{
		std::cout << "Could not export tilemap" << std::endl;
		return 1;
	}

	////////////////////////////////////////

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
			return 1;
		}
	}

	////////////////////////////////////////

	std::cout << "Done" << std::endl;
	return 0;
}

