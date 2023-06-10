
#define LOG_MODULE LOG_MODULE_NONE
#include "Log.h"

#include <stdarg.h>
#include <cstdio>

namespace radplot
{

void Log::LogLine(const char* file, LogLevel level, const char *fmt...)
{
    // TODO: only print for enabled module masks
    // TODO: map module masks to module strings, pass mapping to init function or something

    static LogLevel _current_level = LogLevel::Info;

    if (_current_level != LogLevel::Off && level >= _current_level)
    {
        printf("%s: ", file);

        va_list(args);
        va_start(args, fmt);
        vprintf(fmt, args);

        printf("\n");
    }
}

}