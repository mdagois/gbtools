#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstring>

#include "commandline.h"

namespace cli {

////////////////////////////////////////////////////////////////////////////////

static void fillOptionBase(Option& out_option, const char* name, const char* short_name, const char* description, bool required, uint32_t code, ParameterType parameter_type, OptionType option_type)
{
	assert(name != nullptr);
	assert(code != kRemainingCode && code != kIgnoreCode);
	memset(&out_option, 0, sizeof(Option));
	out_option.name = name;
	out_option.short_name = short_name;
	out_option.description = description;
	out_option.code = code;
	out_option.parameter_type = parameter_type;
	out_option.option_type = option_type;
	out_option.required = required;
}

Option OptionCustom(const char* name, const char* short_name, const char* description, bool required, uint32_t code, ParameterType parameter_type)
{
	Option option;
	fillOptionBase(option, name, short_name, description, required, code, parameter_type, OptionType::kCustom);
	return option;
}

Option OptionFlag(const char* name, const char* short_name, const char* description, uint32_t code, bool* write_back_address)
{
	assert(write_back_address != nullptr);
	Option option;
	fillOptionBase(option, name, short_name, description, false, code, ParameterType::kNone, OptionType::kFlag);
	option.flag.write_back_address = write_back_address;
	return option;
}

Option OptionInteger(const char* name, const char* short_name, const char* description, bool required, uint32_t code, int32_t* write_back_address)
{
	assert(write_back_address != nullptr);
	Option option;
	fillOptionBase(option, name, short_name, description, required, code, ParameterType::kRequired, OptionType::kInteger);
	option.integer.write_back_address = write_back_address;
	return option;
}

Option OptionString(const char* name, const char* short_name, const char* description, bool required, uint32_t code, const char** write_back_address)
{
	assert(write_back_address != nullptr);
	Option option;
	fillOptionBase(option, name, short_name, description, required, code, ParameterType::kRequired, OptionType::kString);
	option.string.write_back_address = write_back_address;
	return option;
}

Option OptionStringToInteger(const char* name, const char* short_name, const char* description, bool required, uint32_t code, int32_t* write_back_address, const Mapping* mappings, uint32_t mapping_count)
{
	assert(write_back_address != nullptr);
	assert(mappings != nullptr);
	assert(mapping_count > 0);
	Option option;
	fillOptionBase(option, name, short_name, description, required, code, ParameterType::kRequired, OptionType::kStringToInteger);
	option.string_to_integer.write_back_address = write_back_address;
	option.string_to_integer.mappings = mappings;
	option.string_to_integer.mapping_count = mapping_count;
	return option;
}

////////////////////////////////////////////////////////////////////////////////

Parser::Parser(const char** argv, int32_t argc, const Option* options, uint32_t option_count, const char* description)
: m_arguments(argv)
, m_argument_count(argc)
, m_options(options)
, m_option_count(option_count)
, m_description(description)
, m_option_name_max_len(0)
, m_option_short_name_max_len(0)
, m_option_required_mask(0)
, m_next_argument_index(1)
, m_last_error(Error::kNone)
, m_done_parsing(false)
{
	assert(argv != nullptr);
	assert(argc > 0);
	assert(options != nullptr || option_count == 0);

	for(uint32_t i = 0; i < option_count; ++i)
	{
		m_option_name_max_len = std::max(m_option_name_max_len, static_cast<uint32_t>(strlen(options[i].name)));
		m_option_short_name_max_len = std::max(m_option_short_name_max_len, static_cast<uint32_t>(strlen(options[i].short_name)));
		if(options[i].required)
		{
			m_option_required_mask |= 1 << i;
		}
	}
}

Parser::~Parser()
{
}

bool Parser::getNextOption(uint32_t& out_code, const char*& out_parameter, Error& out_error)
{
#define RETURN_ERROR(type) \
	out_error = Error::type; \
	m_last_error = Error::type; \
	return false;

	out_code = kIgnoreCode;
	out_parameter = nullptr;

	if(m_last_error != Error::kNone)
	{
		out_error = m_last_error;
		return false;
	}

	if(m_arguments == nullptr || m_argument_count == 0)
	{
		RETURN_ERROR(kNoArgumentSpecified)
	}

	if(!m_done_parsing && (m_options == nullptr || m_option_count == 0))
	{
		out_code = kRemainingCode;
		m_done_parsing = true;
		out_error = Error::kNone;
		return true;
	}

	if(m_done_parsing || m_next_argument_index >= m_argument_count)
	{
		if(m_option_required_mask == 0)
		{
			RETURN_ERROR(kNone)
		}
		else
		{
			RETURN_ERROR(kRequiredOptionNotSpecified);
		}
	}

	const char* argument = m_arguments[m_next_argument_index];
	if(argument[0] == '-')
	{
		if(strlen(argument) == 1)
		{
			RETURN_ERROR(kSingleHiphenDetected)
		}

		uint32_t option_index = 0;
		const Option* option = findOption(argument + 1, option_index);
		if(option == nullptr)
		{
			RETURN_ERROR(kUnknownOption)
		}

		m_option_required_mask &= ~(1 << option_index);

		if(option->parameter_type == ParameterType::kRequired)
		{
			const int32_t parameter_index = m_next_argument_index + 1;
			if(parameter_index >= m_argument_count)
			{
				RETURN_ERROR(kCannotFetchParameter)
			}

			const char* const parameter = m_arguments[parameter_index];
			if(parameter[0] == '-' && !isDigit(parameter))
			{
				RETURN_ERROR(kInvalidParameter)
			}

			out_parameter = parameter;
			++m_next_argument_index;
		}

		out_code = option->code;
		switch(option->option_type)
		{
			case OptionType::kCustom:
			{
				break;
			}
			case OptionType::kFlag:
			{
				assert(out_parameter == nullptr);
				*(option->flag.write_back_address) = true;
				break;
			}
			case OptionType::kInteger:
			{
				assert(out_parameter != nullptr);
				if(out_parameter == nullptr || !isDigit(out_parameter))
				{
					RETURN_ERROR(kInvalidParameter)
				}
				*(option->integer.write_back_address) = atoi(out_parameter);
				break;
			}
			case OptionType::kString:
			{
				assert(out_parameter != nullptr);
				*(option->string.write_back_address) = out_parameter;
				break;
			}
			case OptionType::kStringToInteger:
			{
				assert(out_parameter != nullptr);
				if(out_parameter == nullptr)
				{
					RETURN_ERROR(kInternalError)
				}
				const Mapping* match = nullptr;
				for(uint32_t i = 0; i < option->string_to_integer.mapping_count; ++i)
				{
					const Mapping& mapping = option->string_to_integer.mappings[i];
					if(strcmp(mapping.key, out_parameter) == 0)
					{
						match = &mapping;
						break;
					}
				}
				if(match == nullptr)
				{
					RETURN_ERROR(kNoValidMapping)
				}
				*(option->string_to_integer.write_back_address) = match->value;
				break;
			}
			default:
			{
				assert(false);
				break;
			}
		}

		++m_next_argument_index;
	}
	else
	{
		out_code = kRemainingCode;
		m_done_parsing = true;
	}

	out_error = Error::kNone;
	return true;

#undef RETURN_ERROR
}

const char* Parser::getLastErrorMessage()
{
	if(m_last_error == Error::kNone)
	{
		return nullptr;
	}

	const char* message[] =
	{
		"None", // kNone
		"No argument was specified", // kNoArgumentSpecified
		"A hiphen not followed by an option was detected", // kSingleHiphenDetected
		"The option is unknown", // kUnknownOption
		"The parameter cannot be fetched", // kCannotFetchParameter
		"The parameter is invalid", // kInvalidParameter
		"The parameter does not match any acceptable value", // kNoValidMapping
		"A required option was not provided", // kRequiredOptionNotSpecified
		"An internal error occurred", // kInternalError
	};
	static_assert(sizeof(message) / sizeof(message[0]) == static_cast<size_t>(Error::kCount));

	if(m_last_error == Error::kRequiredOptionNotSpecified)
	{
#define ERROR_MANAGEMENT_ERROR "Error handling issue"
		int32_t message_len = sprintf(m_error_message, "%s:", message[static_cast<size_t>(m_last_error)]);
		if(message_len <= 0)
		{
			return ERROR_MANAGEMENT_ERROR;
		}
		for(uint32_t i = 0; i < m_option_count; ++i)
		{
			if((m_option_required_mask & (1 << i)) != 0)
			{
				const int32_t len = sprintf(m_error_message + message_len, " %s", m_options[i].name);
				if(len <= 0)
				{
					return ERROR_MANAGEMENT_ERROR;
				}
				message_len += len;
			}
		}
#undef ERROR_MANAGEMENT_ERROR
	}
	else
	{
		const char* faulty_argument = getRemainingArgumentCount() == 0 ? nullptr : getRemainingArguments()[0];
		if(faulty_argument == nullptr)
		{
			sprintf(m_error_message, "%s", message[static_cast<size_t>(m_last_error)]);
		}
		else
		{
			sprintf(m_error_message, "%s (%s)", message[static_cast<size_t>(m_last_error)], faulty_argument);
		}
	}
	return m_error_message;
}

const char** Parser::getRemainingArguments() const
{
	return getRemainingArgumentCount() == 0 ? nullptr : m_arguments + m_next_argument_index;
}

int32_t Parser::getRemainingArgumentCount() const
{
	return m_argument_count - m_next_argument_index;
}

const Option* Parser::findOption(const char* name, uint32_t& out_index) const
{
	assert(name != nullptr);
	for(uint32_t i = 0; i < m_option_count; ++i)
	{
		const Option* option = m_options + i;
		if(strcmp(option->name, name) == 0 || strcmp(option->short_name, name) == 0)
		{
			out_index = i;
			return option;
		}
	}
	return nullptr;
}

bool Parser::isDigit(const char* str)
{
	assert(str != nullptr);
	assert(strlen(str) > 0);
	if(str[0] == '-' || str[0] == '+')
	{
		++str;
		if(str[0] == '\0')
		{
			return false;
		}
	}
	const size_t str_len = strlen(str);
	for(size_t i = 0; i < str_len; ++i)
	{
		if(!std::isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

static const char* shortenExecutableName(const char* name)
{
	assert(name != nullptr);

	const char* shortest = name;
	size_t shortest_len = strlen(shortest);

	const char* slash = strrchr(name, '/');
	if(slash != nullptr && strlen(slash) < shortest_len)
	{
		shortest = slash + 1;
		shortest_len = strlen(shortest);
	}

	const char* backslash = strrchr(name, '\\');
	if(backslash != nullptr && strlen(backslash) < shortest_len)
	{
		shortest = backslash + 1;
	}

	return shortest;
}

void Parser::printHelp() const
{
	const char* short_name = shortenExecutableName(m_arguments[0]);
	if(m_description == nullptr)
	{
		printf("%s\n", short_name);
	}
	else
	{
		printf("%s %s\n", short_name, m_description);
	}
	for(uint32_t i = 0; i < m_option_count; ++i)
	{
		const Option& option = m_options[i];

		const char* arg = "";
 		if(option.parameter_type == ParameterType::kRequired)
		{
			arg = "<param>";
		}
		const char* desc = "";
		if(option.description != nullptr)
		{
			desc = option.description;
		}
		const char* required = "";
		if(option.required)
		{
			required = "<req>";
		}

		const uint32_t kShortOptionSpace = m_option_short_name_max_len + 2;
		const uint32_t kOptionSpace = m_option_name_max_len + 2;
		const uint32_t kArgSpace = 10;
		const uint32_t kTotalSpace = kShortOptionSpace + kOptionSpace + kArgSpace + kArgSpace;
		printf("  -%-*s -%-*s %-*s %-*s%s\n", kShortOptionSpace, option.short_name, kOptionSpace, option.name, kArgSpace, arg, kArgSpace, required, desc);
		if(option.option_type == OptionType::kStringToInteger)
		{
			for(uint32_t j = 0; j < option.string_to_integer.mapping_count; ++j)
			{
				const Mapping& mapping = option.string_to_integer.mappings[j];
				if(mapping.desc == nullptr)
				{
					printf("       %-*s* %s\n", kTotalSpace, "", mapping.key);
				}
				else
				{
					printf("       %-*s* %s: %s\n", kTotalSpace, "", mapping.key, mapping.desc);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

}

