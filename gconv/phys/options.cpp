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
			GFX_LOG_ERROR("Unspected error when matching the division sequence [" << sequence << "]");
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

	const char* divisions = nullptr;
	out_is_help = false;

	enum : uint32_t
	{
		kOptionInputFilename,
		kOptionDivisions,
		kOptionOutputFilename,
		kOptionVerbose,
		kOptionHelp,
	};

	Option cli_options[] =
	{
		// input
		OptionString("input", "i", "The input image", true, kOptionInputFilename, &out_options.input.filename),
		OptionString("divisions", "d", "The input division", false, kOptionDivisions, &divisions),

		// output
		OptionString("output", "o", "The output data", false, kOptionOutputFilename, &out_options.output.filename),

		// misc
		OptionFlag("verbose", "v", "Enable verbose mode", kOptionVerbose, &out_options.verbose),
		OptionFlag("help", "h", "Show help", kOptionHelp, &out_options.help),
	};

	Parser cli_parser(
		argv, argc,
		cli_options, sizeof(cli_options) / sizeof(cli_options[0]),
		"--input <filename> --output <filename>");

	uint32_t code;
	const char* parameter;
	Error error;
	while(cli_parser.getNextOption(code, parameter, error))
	{
		switch(code)
		{
			case kRemainingCode:
			{
				GFX_LOG_ERROR("Unsupported trailing parameters");
				return false;
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

	if( divisions != nullptr &&
		!parseDivisions(out_options.input.divisions, divisions))
	{
		GFX_LOG_ERROR("Cannot parse input divisions");
		return false;
	}

	return true;
}

