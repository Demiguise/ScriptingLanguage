#include "common.h"
#include <stdarg.h>

constexpr int MAX_FORMAT_LEN = 256;
std::string Util::Format(const char* frmt, ...)
{
    char buffer[MAX_FORMAT_LEN];

    va_list args;
    va_start(args, frmt);
    auto bytesWritten = vsnprintf(buffer, MAX_FORMAT_LEN, frmt, args);
    va_end(args);

    std::string result;
    result.assign(buffer, bytesWritten);
    return result;
}
