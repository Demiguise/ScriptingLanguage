#ifndef __KEYWORDS_H__
#define __KEYWORDS_H__

#include <string_view>

enum class Keyword
{
    Null,

    If,
    Else,
    Elseif,
    Switch,
    Case,
    Using,
};

Keyword IsKeyword(std::string_view str);

#endif //~__KEYWORDS_H__
