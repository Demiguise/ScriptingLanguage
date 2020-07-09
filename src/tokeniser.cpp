#include "tokeniser.h"

Tokeniser::Tokeniser(std::string filePath)
  : mStream(filePath)
{
}

int Tokeniser::SkipToNewLine()
{
  int count = 0;
  char ch;

  do
  {
    mStream.get(ch);
    count++;
  }
  while (ch != '\r');

  if (mStream.peek() == '\n')
  {
    //Consume the trailing \n in CRLF files
    mStream.get(ch);
    count++;
  }

  return count;
}

std::optional<int> Tokeniser::Parse(std::string& outStatement, TTokenVec& outTokens)
{
  int tokenBegin = 1;
  int tokenEnd = 0;

  int column = mCurColumn;
  int line = mCurLine;

  char ch;
  bool bDone = false;

  //Ensure the statement we're about to write is cleared
  outStatement.clear();
  outTokens.clear();

  if (mStream.eof())
  {
    return -1;
  }

  bool bStringLiteral = false;

  auto addToken = [&](Token type)
  {
    TokenInfo info;
    info.mLine = line;
    info.mColBegin = tokenBegin;
    info.mColEnd = tokenEnd;

    outStatement += ch;
    outTokens.push_back({type, info});

    tokenBegin = tokenEnd;
  };
  #define MAP_SINGLE_TOKEN(char, tokenType) case char: addToken(tokenType); break;

  while (!bDone)
  {
    mStream.get(ch);

    if (mStream.eof())
    {
      //We're completely done but there's no error here yet.
      return {};
    }

    switch (ch)
    {
      case '\"':
      {
        bStringLiteral = !bStringLiteral;
        addToken(Token::Double_Quote);
        break;
      }

      case '\'':
      {
        bStringLiteral = !bStringLiteral;
        addToken(Token::Single_Quote);
        break;
      }

      case '/':
      {
        if (bStringLiteral)
        {
          outStatement += ch;
          tokenEnd++;
        }
        else if (mStream.peek() == '/')
        {
          //This is a comment
          mStream.get(ch);
          tokenEnd += SkipToNewLine();
          addToken(Token::Comment);
        }
        else
        {
          //This is a divide sign
          addToken(Token::Divide);
        }
      }

      MAP_SINGLE_TOKEN(',', Token::Comma);

      MAP_SINGLE_TOKEN('=', Token::Equals);
      MAP_SINGLE_TOKEN('+', Token::Addition);
      MAP_SINGLE_TOKEN('-', Token::Subtraction);
      MAP_SINGLE_TOKEN('*', Token::Multiply);

      MAP_SINGLE_TOKEN('(', Token::Paren_Close);
      MAP_SINGLE_TOKEN(')', Token::Paren_Close);
      MAP_SINGLE_TOKEN('{', Token::Curly_Open);
      MAP_SINGLE_TOKEN('}', Token::Curly_Close);
      MAP_SINGLE_TOKEN('[', Token::Bracket_Open);
      MAP_SINGLE_TOKEN(']', Token::Bracket_Close);
      
      case ' ':
      {
        if (bStringLiteral)
        {
          //String literals can have spaces in them
          outStatement += ch;
        }
        else
        {
          addToken(Token::Literal);
        }

        break;
      }

      case '\r':
      {
        if (mStream.peek() == '\n')
        {
          //Consume the trailing \n in CRLF files
          mStream.get(ch);
        }

        line++;
        mCurColumn = 0;
      }

      case ';':
      {
        //Statement ends;
        bDone = true;
        addToken(Token::Statement_End);
        break;
      }

      //Any other ASCII character
      default:
      {
        if ((ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z'))
        {
          outStatement += ch;
          tokenEnd++;
        }
        else
        {
          //Unhandled case?
          return -1;
        }
        break;
      }
    }
  }

  return {};
}
