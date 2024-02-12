#pragma

#include <sstream>

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////

#define LOG(level, message) gbgfx::log(level, (std::stringstream() << message).str().c_str())
#define LOG_ERROR(message) LOG(gbgfx::kLogLevel_Error, message)
#define LOG_WARN(message) LOG(gbgfx::kLogLevel_Warn, message)
#define LOG_INFO(message) LOG(gbgfx::kLogLevel_Info, message)

enum LogLevel : uint32_t
{
	kLogLevel_None,
	kLogLevel_Error,
	kLogLevel_Warn,
	kLogLevel_Info,
	kLogLevel_All = kLogLevel_Info,
};

void setLogLevel(LogLevel level);
void log(LogLevel level, const char* message);

////////////////////////////////////////////////////////////////////////////////

}

