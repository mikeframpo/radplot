
#include "Radplot.h"

#include <cstdarg>
#include <cstdio>
#include "Log.h"

namespace radplot
{

unsigned int _moduleMask = 0;
LogLevel _level = LogLevel::Off;

const char* GetModuleName(LogModule module)
{
    switch (module)
    {
    case LogModule::Figure:
        return "Figure";
    case LogModule::Window:
        return "Window";
    default:
        return "Unknown";
    }
}

const char* GetLevelStr(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Error:
        return "[Error]";
    case LogLevel::Info:
        return "[Info]";
    case LogLevel::Debug:
        return "[Debug]";
    case LogLevel::Trace:
        return "[Trace]";
    default:
        return "[Unknown]";
    }
}

void LogSetLevel(LogLevel level)
{
    _level = level;
}

void LogEnableModule(LogModule module)
{
    _moduleMask |= static_cast<unsigned int>(module);
}

void LogLineImpl(LogModule module, LogLevel level, const char *format...)
{
    if (_level != LogLevel::Off && level >= _level)
    {
        printf("%s:%s: ", GetLevelStr(level), GetModuleName(module));

        va_list(args);
        va_start(args, format);
        vprintf(format, args);

        printf("\n");
    }
}

}