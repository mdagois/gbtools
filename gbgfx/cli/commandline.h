#pragma once

#include <cstdint>

namespace cli {

////////////////////////////////////////////////////////////////////////////////

static const uint32_t kRemainingCode = '????';
static const uint32_t kIgnoreCode = '@@@@';

enum class ParameterType : uint16_t
{
	kNone,
	kRequired,
};

enum class OptionType : uint16_t
{
	kCustom,
	kFlag,
	kInteger,
	kString,
	kStringToInteger,
};

enum class Error : uint32_t
{
	kNone,
	kNoArgumentSpecified,
	kSingleHiphenDetected,
	kUnknownOption,
	kCannotFetchParameter,
	kInvalidParameter,
	kNoValidMapping,
	kRequiredOptionNotSpecified,
	kInternalError,
	kCount,
};

////////////////////////////////////////////////////////////////////////////////

struct Mapping
{
	const char* key;
	const char* desc;
	int32_t value;
};

struct Option
{
	const char* name;
	const char* short_name;
	const char* description;
	uint32_t code;
	ParameterType parameter_type;
	OptionType option_type;
	bool required;

	union
	{
		struct
		{
			bool* write_back_address;
		} flag;
		struct
		{
			int32_t* write_back_address;
		} integer;
		struct
		{
			const char** write_back_address;
		} string;
		struct
		{
			int32_t* write_back_address;
			const Mapping* mappings;
			uint32_t mapping_count;
		} string_to_integer;
	};
};

////////////////////////////////////////////////////////////////////////////////

Option OptionCustom(const char* name, const char* short_name, const char* description, bool required, uint32_t code, ParameterType parameter_type);
Option OptionFlag(const char* name, const char* short_name, const char* description, uint32_t code, bool* write_back_address);
Option OptionInteger(const char* name, const char* short_name, const char* description, bool required, uint32_t code, int32_t* write_back_address);
Option OptionString(const char* name, const char* short_name, const char* description, bool required, uint32_t code, const char** write_back_address);
Option OptionStringToInteger(const char* name, const char* short_name, const char* description, bool required, uint32_t code, int32_t* write_back_address, const Mapping* mappings, uint32_t mapping_count);

////////////////////////////////////////////////////////////////////////////////

class Parser
{
public:
	Parser(const char** argv, int32_t argc, const Option* options, uint32_t option_count, const char* description);
	~Parser();

	bool getNextOption(uint32_t& out_code, const char*& out_parameter, Error& out_error);
	const char* getLastErrorMessage();
	const char** getRemainingArguments() const;
	int32_t getRemainingArgumentCount() const;

	void printHelp() const;

private:
	const Option* findOption(const char* name, uint32_t& out_index) const;
	static bool isDigit(const char* name);

private:
	const char** m_arguments;
	const int32_t m_argument_count;
	const Option* m_options;
	const uint32_t m_option_count;

	const char* m_description;
	uint32_t m_option_name_max_len;
	uint32_t m_option_short_name_max_len;
	uint32_t m_option_required_mask;

	int32_t m_next_argument_index;
	Error m_last_error;
	bool m_done_parsing;

	char m_error_message[512];
};

////////////////////////////////////////////////////////////////////////////////

}

