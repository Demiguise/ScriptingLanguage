#include "tokeniser.h"

#include <array>
#include <functional>
#include <limits.h>

#include <stdio.h>
#include <iostream>

enum class State
{
  Normal,
  Literal,
  StringLiteral
};

#define OUTPUT_TYPE(type) case type: return #type
std::string TypeToString(Token type)
{
  switch (type)
  {
    OUTPUT_TYPE(Token::Null);
    OUTPUT_TYPE(Token::EndOfStream);
    OUTPUT_TYPE(Token::Comment);
    OUTPUT_TYPE(Token::Literal);
    OUTPUT_TYPE(Token::Double_Quote);
    OUTPUT_TYPE(Token::Single_Quote);
    OUTPUT_TYPE(Token::Comma);
    OUTPUT_TYPE(Token::Equals);
    OUTPUT_TYPE(Token::Addition);
    OUTPUT_TYPE(Token::Subtraction);
    OUTPUT_TYPE(Token::Multiply);
    OUTPUT_TYPE(Token::Divide);
    OUTPUT_TYPE(Token::Paren_Open);
    OUTPUT_TYPE(Token::Paren_Close);
    OUTPUT_TYPE(Token::Curly_Open);
    OUTPUT_TYPE(Token::Curly_Close);
    OUTPUT_TYPE(Token::Bracket_Open);
    OUTPUT_TYPE(Token::Bracket_Close);
    OUTPUT_TYPE(Token::Statement_End);
    default: return "Unknown";
  };
}
#undef OUTPUT_TYPE

Tokeniser::Tokeniser(std::string filePath)
  : mStream(filePath)
{
}

std::optional<int> Tokeniser::Parse_Internal(std::string& outStatement, TTokenVec& outTokens)
{
  char ch;
  int line = mCurLine;
  bool bDone = false;
  State state = State::Normal;

  IndexPair strIdx;

  //Ensure the statement we're about to write is cleared
  outStatement.clear();
  outTokens.clear();

  if (mStream.eof())
  {
    return -1;
  }

  auto addToken = [&](Token type)
  {
    TokenInfo info;
    info.mLine = line;
    info.mStr = strIdx;

    std::cout << "Adding " << TypeToString(type) << " [" << 
              strIdx.begin << ":" << strIdx.end << "] " << 
              outStatement.substr(strIdx.begin, strIdx.end - strIdx.begin) << std::endl;
    outTokens.push_back({type, info});

    strIdx.begin = strIdx.end;
  };

  using TCharHandler = std::function<int()>;
  auto defaultHandler = []() -> int{
    return -1;
  };

  auto textHandler = [&]() ->int {
    if (state == State::Normal)
    {
      state = State::Literal;
    }

    outStatement += ch;
    strIdx.end++;
    return 0;
  };

  //Create a handler for each possible ASCII character
  std::array<TCharHandler, UCHAR_MAX> handlers;
  handlers.fill(defaultHandler);

  //Opening/Closing string literals
  handlers['\"'] = [&]() -> int {
    state = (state == State::Normal) ? State::StringLiteral : State::Normal;
    outStatement += ch;
    addToken(Token::Double_Quote);
    return 0;
  };

  handlers['\''] = [&]() -> int {
    state = (state == State::Normal) ? State::StringLiteral : State::Normal;
    outStatement += ch;
    addToken(Token::Single_Quote);
    return 0;
  };

  //Comments
  handlers['/'] = [&]() -> int {
    if (state == State::StringLiteral)
    {
      outStatement += ch;
      strIdx.end++;
    }
    else if (mStream.peek() == '/')
    {
      //This is a comment
      std::cout << "Encountered a comment, skipping to newline" << std::endl;

      do
      {
        outStatement += ch;
        strIdx.end++;
        mStream.get(ch);
      } while (ch != '\r' && ch != '\n');

      if (mStream.peek() == '\n')
      {
        //Consume the trailing \n in CRLF files
        mStream.get(ch);
        strIdx.end++;
      }

      addToken(Token::Comment);
      bDone = true;
    }
    else
    {
      //This is a divide sign
      outStatement += ch;
      addToken(Token::Divide);
    }
    return 0;
  };

  //New lines
  handlers['\r'] = [&]() -> int {
    if (mStream.peek() == '\n')
    {
      //Consume the trailing \n in CRLF files
      mStream.get(ch);
    }

    line++;
    return 0;
  };

  handlers['\n'] = [&]() -> int {
    line++;
    return 0;
  };

  //Whitespace
  handlers[' '] = [&]() -> int {
    if (state == State::StringLiteral)
    {
      //String literals can have spaces in them
      outStatement += ch;
      strIdx.end++;
    }
    else if (state == State::Literal)
    {
      /*
        If we've hit whitespace, we want to just add whatever came before as a literal.
        We have no idea if this is a valid literal just yet.
      */
      outStatement += ch;
      addToken(Token::Literal);

      //Now increment ourselves past the whitespace.
      strIdx.begin++;
      strIdx.end++;
      state = State::Normal;
    }
    else
    {
      //Ignore it
      std::cout << "Skipping whitespace" << std::endl;
      outStatement += ch;
      strIdx.begin++;
      strIdx.end++;
    }

    return 0;
  };

  //Operators
  handlers[';'] = [&]() -> int {
    bDone = true;
    if (state == State::Literal)
    {
      addToken(Token::Literal);
    }

    outStatement += ch;
    addToken(Token::Statement_End);
    return 0;
  };

  #define ADD_OPERATOR_HANDLER(char, singleType) \
    handlers[char] = [&]() -> int { \
      strIdx.end++; \
      outStatement += ch; \
      addToken(singleType); \
      return 0; \
    }

  ADD_OPERATOR_HANDLER(',', Token::Comma);
  ADD_OPERATOR_HANDLER('=', Token::Equals);
  ADD_OPERATOR_HANDLER('+', Token::Addition);
  ADD_OPERATOR_HANDLER('-', Token::Subtraction);
  ADD_OPERATOR_HANDLER('+', Token::Addition);
  ADD_OPERATOR_HANDLER('(', Token::Paren_Open);
  ADD_OPERATOR_HANDLER(')', Token::Paren_Close);
  ADD_OPERATOR_HANDLER('{', Token::Curly_Open);
  ADD_OPERATOR_HANDLER('}', Token::Curly_Close);
  ADD_OPERATOR_HANDLER('[', Token::Bracket_Open);
  ADD_OPERATOR_HANDLER(']', Token::Bracket_Close);

  //Lower case letters
  handlers['a'] = textHandler;
  handlers['b'] = textHandler;
  handlers['c'] = textHandler;
  handlers['d'] = textHandler;
  handlers['e'] = textHandler;
  handlers['f'] = textHandler;
  handlers['g'] = textHandler;
  handlers['h'] = textHandler;
  handlers['i'] = textHandler;
  handlers['j'] = textHandler;
  handlers['k'] = textHandler;
  handlers['l'] = textHandler;
  handlers['m'] = textHandler;
  handlers['n'] = textHandler;
  handlers['o'] = textHandler;
  handlers['p'] = textHandler;
  handlers['q'] = textHandler;
  handlers['r'] = textHandler;
  handlers['s'] = textHandler;
  handlers['t'] = textHandler;
  handlers['u'] = textHandler;
  handlers['v'] = textHandler;
  handlers['w'] = textHandler;
  handlers['x'] = textHandler;
  handlers['y'] = textHandler;
  handlers['z'] = textHandler;

  //Upper case letters
  handlers['A'] = textHandler;
  handlers['B'] = textHandler;
  handlers['C'] = textHandler;
  handlers['D'] = textHandler;
  handlers['E'] = textHandler;
  handlers['F'] = textHandler;
  handlers['G'] = textHandler;
  handlers['H'] = textHandler;
  handlers['I'] = textHandler;
  handlers['J'] = textHandler;
  handlers['K'] = textHandler;
  handlers['L'] = textHandler;
  handlers['M'] = textHandler;
  handlers['N'] = textHandler;
  handlers['O'] = textHandler;
  handlers['P'] = textHandler;
  handlers['Q'] = textHandler;
  handlers['R'] = textHandler;
  handlers['S'] = textHandler;
  handlers['T'] = textHandler;
  handlers['U'] = textHandler;
  handlers['V'] = textHandler;
  handlers['W'] = textHandler;
  handlers['X'] = textHandler;
  handlers['Y'] = textHandler;
  handlers['Z'] = textHandler;

  //Numbers
  handlers['0'] = textHandler;
  handlers['1'] = textHandler;
  handlers['2'] = textHandler;
  handlers['3'] = textHandler;
  handlers['4'] = textHandler;
  handlers['5'] = textHandler;
  handlers['6'] = textHandler;
  handlers['7'] = textHandler;
  handlers['8'] = textHandler;
  handlers['9'] = textHandler;

  while (!bDone)
  {
    mStream.get(ch);

    if (mStream.eof())
    {
      //We're completely done but there's no error here yet. Add a literal as best guess?
      addToken(Token::Literal);
      return {};
    }

    if (handlers[ch]() < 0)
    {
      return -1;
    }
  }

  return {};
}

std::optional<int> Tokeniser::Parse(std::string& outStatement, TTokenVec& outTokens)
{
  auto err = Parse_Internal(outStatement, outTokens);
  if (err.has_value())
  {
    return err;
  }

  //Map token raw views to the statement.
  for (auto& [type, token] : outTokens)
  {
    int tokenLen = token.mStr.end - token.mStr.begin;
    token.mRaw = std::string_view(outStatement.c_str() + token.mStr.begin, tokenLen);
  }

  return {};
}
