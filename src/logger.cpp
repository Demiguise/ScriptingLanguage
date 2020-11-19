#include "logger.h"

#include <stdarg.h>
#include <unordered_map>
#include <stdio.h>
#include <deque>
#include <fstream>

struct SModuleMap
{
    std::vector<CModuleLogger*> m_loggers;
};

SModuleMap* GetModuleMap(EModule mod)
{
    static std::unordered_map<EModule, SModuleMap> modMap = {};
    return &modMap[mod];
}

struct SLoggerContext
{
    static const int MaxLogLength = 256;
    std::string m_logBasePath;
} g_ctx;

const char* GetModuleString(EModule mod)
{
    switch (mod)
    {
    case EModule::System: return "System";
    case EModule::Rendering: return "Rendering";
    case EModule::Simulation: return "Simulation";
    case EModule::Input: return "Input";
    case EModule::ScriptLang: return "ScriptLang";
    default: return "Unknown";
    }
}

const char* GetLevelString(ELogLevel mod)
{
    switch (mod)
    {
    case ELogLevel::Error: return "Error";
    case ELogLevel::Warning: return "Warning";
    case ELogLevel::Info: return "Info";
    default: return "Unknown";
    }
}

EModule ModuleFromString(std::string str)
{
    if (str.compare("System") == 0)
    {
        return EModule::System;
    }
    else if (str.compare("Rendering") == 0)
    {
        return EModule::Rendering;
    }
    else if (str.compare("Simulation") == 0)
    {
        return EModule::Simulation;
    }
    else if (str.compare("Input") == 0)
    {
        return EModule::Input;
    }
    else if (str.compare("ScriptLang") == 0)
    {
        return EModule::ScriptLang;
    }
    else
    {
        return EModule::System;
    }
}

ELogLevel LogLevelFromString(std::string str)
{
    if (str.compare("Error") == 0)
    {
        return ELogLevel::Error;
    }
    else if (str.compare("Warning") == 0)
    {
        return ELogLevel::Warning;
    }
    else if (str.compare("Info") == 0)
    {
        return ELogLevel::Info;
    }
    else
    {
        return ELogLevel::Error;
    }
}

void Logging::Initialise(std::string path, std::string cfgPath)
{
    g_ctx.m_logBasePath = path;
}

CModuleLogger::CModuleLogger(EModule mainModule, const char* subModule)
    : m_mainModule(mainModule)
    , m_subModule(subModule)
    , m_logLevel(ELogLevel::Info)
{
}

void CModuleLogger::Log(ELogLevel level, const char* frmt, ...)
{
    char buffer[SLoggerContext::MaxLogLength];

    if (m_logLevel < level)
    {
        return;
    }

    //TODO: Add time to the log.
    int bytesWritten = snprintf(buffer, SLoggerContext::MaxLogLength, "[%s:%s] %s | ",
        GetModuleString(m_mainModule), m_subModule.c_str(), GetLevelString(level));

    va_list args;
    va_start(args, frmt);
    bytesWritten += vsnprintf(buffer + bytesWritten, SLoggerContext::MaxLogLength - bytesWritten, frmt, args);
    va_end(args);

    //Add the null terminator.
    buffer[bytesWritten] = '\n';
    buffer[bytesWritten + 1] = '\0';
}

void CModuleLogger::SetLogLevel(ELogLevel newLevel)
{
    m_logLevel = newLevel;
}
