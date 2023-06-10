#pragma once

namespace radplot
{

enum class LogLevel
{
    Trace = 0,
    Debug,
    Info,
    Error,

    Off,
};

// TODO: make a Singleton template class
class Log
{
public:
    static void LogLine(const char* module, LogLevel level, const char* fmt...);
};

#ifndef LOG_MODULE
#error "LOG_MODULE must be defined prior to #including Log.h"
#endif

#define LOG_INFO(...) Log::LogLine(LOG_MODULE, LogLevel::Info, __VA_ARGS__)

}
