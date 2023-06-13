
#include "Log.h"

#include <cstdarg>
#include <cstdio>

#include "Radplot.h"

namespace radplot {

unsigned int _moduleMask = 0;
LogLevel _level = LogLevel::Off;

const char* GetLevelStr(LogLevel level) {
    switch (level) {
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

#define X(module, mask)     \
    case LogModule::module: \
        return #module;

// Generate string names for each module
const char* GetModuleName(LogModule module) {
    switch (module) {
        LOG_MODULES
        default:
            return "Unknown";
    }
}
#undef X

void LogSetLevel(LogLevel level) { _level = level; }

void LogEnableModule(LogModule module) { _moduleMask |= static_cast<unsigned int>(module); }

void LogEnableAllModules() { _moduleMask = ~(0u); }

void LogLineImpl(LogModule module, LogLevel level, const char* format, va_list args) {
    if (_level != LogLevel::Off && level >= _level) {
        printf("[%s]:%s:", GetLevelStr(level), GetModuleName(module));
        vprintf(format, args);
    }
}

}  // namespace radplot