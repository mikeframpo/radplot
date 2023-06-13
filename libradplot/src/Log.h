#pragma once

#include <cstdarg>
#include <iostream>

namespace radplot
{

// TODO: look into X macros for generating strings
enum class LogModule
{
    Figure      = (1 << 0),
    Window      = (1 << 1)
};

enum class LogLevel
{
    Trace = 0,
    Debug,
    Info,
    Error,

    Off,
};

extern unsigned int _moduleMask;
extern LogLevel _level;

void LogSetLevel(LogLevel level);
void LogEnableModule(LogModule module);
void LogLineImpl(LogModule module, LogLevel level, const char* format...);

const char* GetModuleName(LogModule module);
const char* GetLevelStr(LogLevel level);

template <LogModule module>
class Logger
{
public:
    static void LogLine(LogLevel level, const char* format...)
    {
        // TODO: perhaps just pass the va_list to the log function?
        if (_moduleMask & static_cast<unsigned int>(module) &&
            _level != LogLevel::Off &&
            level >= _level)
        {
            char buffer[256];
            char* pbuf = buffer;

            int offs = sprintf(pbuf, "%s:%s:", GetLevelStr(level), GetModuleName(module));
            pbuf += offs;

            va_list args;
            va_start(args, format);
            vsnprintf(pbuf, sizeof(buffer) - offs, format, args);

            std::cout << buffer << '\n';
        }
    }
};


#ifdef LOG_MODULE
#error "LOG_MODULE should only be defined once."
#endif

#define LOG_MODULE(module)  using Log = radplot::Logger<module>;

#define LOG_INFO(...) Log::LogLine(LogLevel::Info, __VA_ARGS__)
#define LOG_ERROR(...) Log::LogLine(LogLevel::Error, __VA_ARGS__)

}
