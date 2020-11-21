#include "keywords.h"

#include <vector>

using TKeywords = std::vector<std::pair<std::string, Keyword>>;
TKeywords gKeywordMap = {
    { "if", Keyword::If},
    { "else", Keyword::Else},
    { "elseif", Keyword::Elseif},
    { "switch", Keyword::Switch},
    { "case", Keyword::Case},
    { "using", Keyword::Using},
};

Keyword IsKeyword(std::string_view str)
{
    for (auto& keyword : gKeywordMap)
    {
        if (str == keyword.first)
        {
            return keyword.second;
        }
    }

    return Keyword::Null;
}
