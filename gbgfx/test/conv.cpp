#include <cassert>
#include <iostream>
#include <vector>

#include "commandline.h"
#include "gbgfx.h"

////////////////////////////////////////////////////////////////////////////////

namespace gfx = ::gbgfx;

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

enum Hardware : int32_t
{
	kHardwareDmg,
	kHardwareCgb,
};

enum TileRemoval : int32_t
{
	kTileRemoval_None,
	kTileRemoval_Doubles,
	kTileRemoval_Flips,
};

struct Options
{
	Hardware hardware = kHardwareDmg;

	struct
	{
		const char* filename = nullptr;
		int32_t start_tile_row = 0;
		int32_t tile_row_count = gfx::kIterateAllRows;
		int32_t metatile_width = gfx::kTileSize;
		int32_t metatile_height = gfx::kTileSize;
		bool use_microtile_8x16 = false;
		TileRemoval tile_removal = kTileRemoval_None;
	}
	tileset;

	struct
	{
		std::vector<const char*> filenames;
		bool use_flips = false;
	}
	tilemap;

	struct
	{
		bool add_binary_headers = false;
		uint32_t palette_offset_index = 0;
		uint32_t palette_max_count = 256;
		bool output_palette = true;
		bool output_tileset = true;
		bool output_tilemap_indices = true;
		bool output_tilemap_parameters = true;
		bool use_tile_bank = false;
		bool use_8800_addressing_mode = false;
	}
	output;

	bool verbose = false;
	bool help = false;
};

////////////////////////////////////////////////////////////////////////////////
// CLI
////////////////////////////////////////////////////////////////////////////////

static bool parseCliOptions(Options& out_options, int& out_ret, int argc, const char** argv)
{
	using namespace cli;

	out_ret = 0;

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
		OptionFlag("8x16", "Use this for 8x16 sprite tiles", '8x16', &out_options.tileset.use_microtile_8x16),
		OptionStringToInteger(
			"tr", "Set tile removal mode", false, 'TREM', reinterpret_cast<int32_t*>(&out_options.tileset.tile_removal),
			tile_removal_mapping, sizeof(tile_removal_mapping) / sizeof(tile_removal_mapping[0])),

		// tilemap
		OptionFlag("fl", "Use flips when exporting tilemaps", 'FLIP', &out_options.tilemap.use_flips),

		// output
		OptionFlag("bh", "Add headers to output files", 'HEAD', &out_options.output.add_binary_headers),

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
				out_options.tileset.filename = arguments[0];
				for(int32_t i = 1; i < cli_parser.getRemainingArgumentCount(); ++i)
				{
					out_options.tilemap.filenames.push_back(arguments[i]);
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
		return false;
	}

	if(error != Error::kNone)
	{
		std::cout << cli_parser.getLastErrorMessage() << std::endl;
		out_ret = 1;
		return false;
	}

	if(out_options.tileset.filename == nullptr)
	{
		cli_parser.printHelp();
		return false;
	}

	gfx::setLogLevel(out_options.verbose ? gfx::kLogLevel_Info : gfx::kLogLevel_Error);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Data export
////////////////////////////////////////////////////////////////////////////////

static bool loadDataFromImages(
	gfx::Tileset& out_tileset, gfx::PaletteSet& out_palette_set,
	std::vector<gfx::Tilemap> out_tilemaps,
	const Options& options)
{
	if(!gfx::extractTileset(
		out_tileset, out_palette_set,
		options.tileset.start_tile_row, options.tileset.tile_row_count,
		options.tileset.metatile_width, options.tileset.metatile_height,
		options.tileset.use_microtile_8x16,
		options.tileset.tile_removal != kTileRemoval_None,
		options.tileset.tile_removal == kTileRemoval_Flips,
		options.tileset.filename))
	{
		std::cout
			<< "Could not extract tileset from [" << options.tileset.filename << "]"
			<< std::endl;
		return false;
	}

	out_tilemaps.clear();
	for(auto filename : options.tilemap.filenames)
	{
		const size_t size = out_tilemaps.size();
		out_tilemaps.resize(size + 1);
		if(!gfx::extractTilemap(
			out_tilemaps[size], out_tileset, out_palette_set,
			options.tilemap.use_flips, filename))
		{
			std::cout
				<< "Could not extract tilemap from [" << filename << "]"
				<< std::endl;
			return false;
		}
	}

	return true;
}

static bool exportData(const Options& options)
{
	gfx::Tileset tileset;
	gfx::PaletteSet palette_set;
	std::vector<gfx::Tilemap> tilemaps;
	if(!loadDataFromImages(tileset, palette_set, tilemaps, options))
	{
		return false;
	}

	////////////////////////////////////////

#if 0
	if(!gfx::exportPaletteSet(palette_set, "test/palette.pal", options.output.add_binary_headers))
	{
		std::cout << "Could not export palette set" << std::endl;
		return false;
	}

	if(!gfx::exportTileset(tileset, "test/tileset.chr", options.output.add_binary_headers))
	{
		std::cout << "Could not export tileset" << std::endl;
		return false;
	}

	if(!gfx::exportTilemap(
		tilemap, "test/tilemap.idx", "test/tilemap.prm",
		options.output.add_binary_headers, 0, 128))
	{
		std::cout << "Could not export tilemap" << std::endl;
		return false;
	}
#endif

	////////////////////////////////////////

#if 0
	for(uint32_t i = 0; i < gfx::kTileFlipType_Count; ++i)
	{
		static const char* filenames[] =
		{
			"test/demo_tileset_0.png",
			"test/demo_tileset_1.png",
			"test/demo_tileset_2.png",
			"test/demo_tileset_3.png",
		};
		static_assert(sizeof(filenames) / sizeof(filenames[0]) == gfx::kTileFlipType_Count);
		if(!gfx::writeTilesetToPNG(
			filenames[i], 16,
			tileset, static_cast<gfx::TileFlipType>(i),
			palette_set, i == 0))
		{
			std::cout << "Could not write tileset" << std::endl;
			return false;
		}
	}
#endif

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

	if(!exportData(options))
	{
		return 1;
	}

	return 0;
}

