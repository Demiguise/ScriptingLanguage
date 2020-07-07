#ifndef __TOKENISER_H__
#define __TOKENISER_H__

#include <istream>

enum class Token
{
  EndOfStream,
  Identifer, //What determines a valid name?

  Scope_Open,
  Scope_Close,

  Equals,
  Addition,
  Subtraction,
  Multiply,
  Divide,
};

class Tokeniser
{
public:
  Tokeniser(std::ifstream& stream);

  Token Next();
};

#endif //__TOKENISER_H__
