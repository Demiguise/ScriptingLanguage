#include "executor.h"

#include <stdio.h>
#include <iostream>

#include <unordered_map>

#include "types.h"

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
{}

using TKeywordMap = std::unordered_map<std::string_view, Type>;
TKeywordMap sTypes = {
  {"int", Type::Int},
  {"bool", Type::Bool},
  {"string", Type::String},
  {"float", Type::Float},
};

bool Executor::Execute()
{
  std::string raw_statement = "";
  TTokenVec tokens;
  while (!mTokeniser.Parse(raw_statement, tokens).has_value())
  {
    std::cout << "RAW: " << raw_statement << std::endl;
    std::cout << "[";
    bool bFirst = true;
    for (const auto& [type, token] : tokens)
    {
      if (!bFirst)
      {
        std::cout << ",";
      }

      std::cout << token.mRaw;
      bFirst = false;
    }
    std::cout << "]" << std::endl << std::flush;

    if (tokens[0].first == Token::Literal)
    {
      auto type = sTypes.find(tokens[0].second.mRaw);
      if (type != sTypes.end())
      {
        //We have a valid type
        if (tokens[1].first == Token::Literal)
        {
          //This MUST be an identifer
          mStack.Create(type->second, tokens[1].second.mRaw);
        }
      }
      else
      {
        //Check against built-in functions
      }

    }
  }

  return true;
}
