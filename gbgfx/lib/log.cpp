#include <cassert>
#include <iostream>

#include "log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

static uint32_t s_log_level = kLogLevel_Error;

////////////////////////////////////////////////////////////////////////////////

void setLogLevel(LogLevel level)
{
	assert(level <= kLogLevel_All);
	s_log_level = level;
}

////////////////////////////////////////////////////////////////////////////////

void log(LogLevel level, const char* message)
{
	assert(level < kLogLevel_All);

	if(level > s_log_level)
	{
		return;
	}

	static const char* headers[] =
	{
		"[NONE]  ",
		"[ERROR] ",
		"[WARN]  ",
		"[INFO]  ",
	};
	static_assert(sizeof(headers) / sizeof(headers[0]) == kLogLevel_All);
	std::cout << headers[level] << message << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

}

