#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>

enum class ELogLevel
{
    Error = 0,
    Warning,
    Info
};

enum class EModule
{
    System = 0,
    Rendering,
    Simulation,
    Input,
    ScriptLang,
};

#define DECLARE_LOGGER(name) extern CModuleLogger name;
#define DEFINE_LOGGER(mainModule, subModule, name) CModuleLogger name(mainModule, subModule);

#define LOG_ERR(name, frmt, ...) name.Log(ELogLevel::Error, frmt, ##__VA_ARGS__)
#define LOG_WARN(name, frmt, ...) name.Log(ELogLevel::Warning, frmt, ##__VA_ARGS__)
#define LOG_INFO(name, frmt, ...) name.Log(ELogLevel::Info, frmt, ##__VA_ARGS__)

class CModuleLogger
{
public:
    CModuleLogger(EModule mainModule, const char* subModule);

    void Log(ELogLevel level, const char* frmt, ...);
    void SetLogLevel(ELogLevel newLevel);

private:
    EModule m_mainModule;
    std::string m_subModule;
    ELogLevel m_logLevel;
};

namespace Logging
{
    void Initialise(std::string path, std::string cfgPath);
}

#endif //~__LOGGER_H__
