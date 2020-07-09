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

    outStatement += ch;
    outTokens.push_back({type, info});

    strIdx.begin = strIdx.end;
  };
  #define MAP_SINGLE_TOKEN(char, tokenType) case char: strIdx.end++; addToken(tokenType); break;

  while (!bDone)
  {
    mStream.get(ch);

    if (mStream.eof())
    {
      //We're completely done but there's no error here yet. Add a literal as best guess?
      addToken(Token::Literal);
      return {};
    }

    switch (ch)
    {
      case '\"':
      {
        state = (state == State::Normal) ? State::StringLiteral : State::Normal;
        addToken(Token::Double_Quote);
        break;
      }

      case '\'':
      {
        state = (state == State::Normal) ? State::StringLiteral : State::Normal;
        addToken(Token::Single_Quote);
        break;
      }

      case '/':
      {
        if (state == State::StringLiteral)
        {
          outStatement += ch;
          strIdx.end++;
        }
        else if (mStream.peek() == '/')
        {
          //This is a comment
          mStream.get(ch);
          strIdx.end += SkipToNewLine();
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
        if (state == State::StringLiteral)
        {
          //String literals can have spaces in them
          outStatement += ch;
        }
        else
        {
          /*
            If we've hit whitespace, we want to just add whatever came before as a literal.
            We have no idea if this is a valid literal just yet.
          */
          addToken(Token::Literal);

          //Now increment ourselves past the whitespace.
          strIdx.begin++;
          strIdx.end++;
        }

        break;
      }

      case '\n':
      case '\r':
      {
        if (mStream.peek() == '\n')
        {
          //Consume the trailing \n in CRLF files
          mStream.get(ch);
        }

        line++;
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
          strIdx.end++;
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
    int tokenLen = token.mStr.begin - token.mStr.end;
    token.mRaw = std::string_view(outStatement.c_str() + (token.mStr.begin - 1), tokenLen);
  }

  return {};
}
