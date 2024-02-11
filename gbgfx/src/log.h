#pragma

#include <sstream>

////////////////////////////////////////////////////////////////////////////////

#define LOG(level, message) log(level, (std::stringstream() << message).str().c_str())
#define LOG_ERROR(message) LOG(kLogLevel_Error, message)
#define LOG_WARN(message) LOG(kLogLevel_Warn, message)
#define LOG_INFO(message) LOG(kLogLevel_Info, message)

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

