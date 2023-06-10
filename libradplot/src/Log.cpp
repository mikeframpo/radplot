
#include "Radplot.h"

#include <stdarg.h>
#include <cstdio>

namespace radplot
{

const char* GetLevelStr(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Error:
        return "Error";
    case LogLevel::Info:
        return "Info";
    case LogLevel::Debug:
        return "Debug";
    case LogLevel::Trace:
        return "Trace";
    default:
        return "Unknown";
    }
}

void Log::LogLine(unsigned int module, LogLevel level, const char *fmt...)
{
    // TODO: only print for enabled module masks
    // TODO: map module masks to module strings, pass mapping to init function or something

    static LogLevel _current_level = LogLevel::Info;

    if (_current_level != LogLevel::Off && level >= _current_level)
    {
        printf("%d:%s: ", module, GetLevelStr(level));

        va_list(args);
        va_start(args, fmt);
        vprintf(fmt, args);

        printf("\n");
    }
}

}