#pragma once

#include <cstdarg>
#include <iostream>

namespace radplot {

// Log Levels
enum class LogLevel {
    Trace = 0,
    Debug,
    Info,
    Error,

    Off,
};

// Module definitions
#define LOG_MODULES \
    X(Figure, 0)    \
    X(Window, 1)    \
    X(Renderer, 2)  \
    X(Shader, 3)

// Generate bitfields for each module
#define X(module, mask) module = (1 << (mask)),
enum class LogModule {
    LOG_MODULES
};
#undef X

#ifdef LOG_MODULE
#error "LOG_MODULE should only be defined once in each compilation unit."
#endif

// Define compilation unit as a module, passing a LogModule::Module value
#define LOG_MODULE(module) using Log = radplot::Logger<module>;

// Convenience logging macros.
#define LOG_TRACE(format, ...) Log::LogLine(LogLevel::Trace, format "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOG_DEBUG(format, ...) Log::LogLine(LogLevel::Debug, format "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO(format, ...) Log::LogLine(LogLevel::Info, format "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(format, ...) Log::LogLine(LogLevel::Error, format "\n" __VA_OPT__(, ) __VA_ARGS__)

#define LOG_TRACE_IF(condition, format, ...) Log::LogLineIf(condition, LogLevel::Trace, format "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOG_DEBUG_IF(condition, format, ...) Log::LogLineIf(condition, LogLevel::Debug, format "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO_IF(condition, format, ...) Log::LogLineIf(condition, LogLevel::Info, format "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR_IF(condition, format, ...) Log::LogLineIf(condition, LogLevel::Error, format "\n" __VA_OPT__(, ) __VA_ARGS__)

// Public functions
void LogSetLevel(LogLevel level);
void LogEnableModule(LogModule module);
void LogEnableAllModules();

// Implementation

extern unsigned int _moduleMask;
extern LogLevel _level;

void LogLineImpl(LogModule module, LogLevel level, const char* format, va_list args);
const char* GetLevelStr(LogLevel level);
const char* GetModuleName(LogModule module);

template <LogModule module>
class Logger {
public:
    static void LogLine(LogLevel level, const char* format...) {
        if (_moduleMask & static_cast<unsigned int>(module)) {
            va_list args;
            va_start(args, format);

            LogLineImpl(module, level, format, args);
            va_end(args);
        }
    }

    static void LogLineIf(bool condition, LogLevel level, const char* format...) {
        if (condition && (_moduleMask & static_cast<unsigned int>(module))) {
            va_list args;
            va_start(args, format);

            LogLineImpl(module, level, format, args);
            va_end(args);
        }
    }
};

}  // namespace radplot
