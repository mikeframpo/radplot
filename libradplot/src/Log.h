#pragma once

namespace radplot
{

#define LOG_MODULE_FIGURE       (1 << 0)

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
    static void LogLine(unsigned int module, LogLevel level, const char* fmt...);
};


#ifndef LOG_MODULE
#define LOG_MODULE(module)  static unsigned int s_module = module;
#endif

#define LOG_INFO(...) Log::LogLine(s_module, LogLevel::Info, __VA_ARGS__)

}
