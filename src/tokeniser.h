#ifndef __TOKENISER_H__
#define __TOKENISER_H__

#include <sstream>
#include <string>
#include <string_view>
#include <list>
#include "common.h"

enum class Token
{
  Null,
  EndOfStream,
  Comment,

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

std::string TypeToString(Token type);
bool IsAnOperator(Token type);

struct IndexPair
{
  int begin = 0;
  int end = 0;
};

struct TokenInfo
{
  int mLine;
  IndexPair mStr; //Maps to the location of the token in the string
  std::string_view mRaw;
};

using TTokenPair = std::pair<Token, TokenInfo>;
using TTokenGroup = std::list<TTokenPair>;

class Tokeniser
{
private:
  std::stringstream mStream;
  int mCurLine = 1;

  Result<TTokenGroup> Parse_Internal(std::string& outStatement);

public:
  Tokeniser();

  Result<bool> SetStream(std::string inString);
  Result<TTokenGroup> Parse(std::string& outStatement);
};

#endif //__TOKENISER_H__
