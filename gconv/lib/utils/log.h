#pragma once

#include <cstdint>
#include <sstream>

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

#define GBGFX_LOG(level, message) gbgfx::log(level, static_cast<std::stringstream&&>((std::stringstream() << message)).str().c_str())
#define GBGFX_LOG_ERROR(message) GBGFX_LOG(gbgfx::kLogLevel_Error, message)
#define GBGFX_LOG_WARN(message) GBGFX_LOG(gbgfx::kLogLevel_Warn, message)
#define GBGFX_LOG_INFO(message) GBGFX_LOG(gbgfx::kLogLevel_Info, message)
#define GBGFX_LOG_DEBUG(message) GBGFX_LOG(gbgfx::kLogLevel_Debug, message)

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
