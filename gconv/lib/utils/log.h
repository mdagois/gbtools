#pragma once

#include <cstdint>
#include <sstream>

namespace gfx {

////////////////////////////////////////////////////////////////////////////////

#define GFX_LOG(level, message) gfx::log(level, static_cast<std::stringstream&&>((std::stringstream() << message)).str().c_str())
#define GFX_LOG_ERROR(message) GFX_LOG(gfx::kLogLevel_Error, message)
#define GFX_LOG_WARN(message) GFX_LOG(gfx::kLogLevel_Warn, message)
#define GFX_LOG_INFO(message) GFX_LOG(gfx::kLogLevel_Info, message)
#define GFX_LOG_DEBUG(message) GFX_LOG(gfx::kLogLevel_Debug, message)

enum LogLevel : uint32_t
{
	kLogLevel_None,
	kLogLevel_Error,
	kLogLevel_Warn,
	kLogLevel_Info,
	kLogLevel_Debug,
	kLogLevel_All,
};

void setLogLevel(LogLevel level);
void log(LogLevel level, const char* message);

////////////////////////////////////////////////////////////////////////////////

}

