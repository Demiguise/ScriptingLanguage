#ifndef __TOKENISER_H__
#define __TOKENISER_H__

#include <istream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

enum class Token
{
  Null,
  EndOfStream,

  Literal,      // Any string of characters A-Za-z0-9
  Double_Quote, // " Character
  Single_Quote, // ' Character
  Comma,        // , character

  Equals,       // =
  Addition,     // +
  Subtraction,  // -
  Multiply,     // *
  Divide,       // /

  Paren_Open,   // (
  Paren_Close,  // )
  Curly_Open,   // {
  Curly_Close,  // }
  Bracket_Open, // [
  Bracket_Close,// ]

  Statement_End,// ;
};

struct TokenInfo
{
  int mLine;
  int mColBegin;
  int mColEnd;
  std::string_view mRaw;
};

using TTokenPair = std::pair<Token, TokenInfo>;
using TTokenVec = std::vector<TTokenPair>;

class Tokeniser
{
private:
  std::ifstream mStream;
  int mCurLine = 1;
  int mCurColumn = 1;

public:
  Tokeniser(std::string filePath);

  std::optional<int> Parse(std::string& outStatement, TTokenVec& outTokens);
};

#endif //__TOKENISER_H__
