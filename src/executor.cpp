#include "executor.h"

#include <stdio.h>
#include <iostream>

#include <vector>
#include <algorithm>

#include "types.h"

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
{}

using TTypeVec = std::vector<Type>;
TTypeVec sTypes = {
  { BaseType::Int,    "int" },
  { BaseType::Bool,   "bool" },
  { BaseType::String, "string" },
  { BaseType::Float,  "float" },
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
      auto type = std::find_if(sTypes.begin(), sTypes.end(), 
        [&](Type& element) { return element.Name() == tokens[0].second.mRaw; });
      if (type != sTypes.end())
      {
        //We have a valid type
        if (tokens[1].first == Token::Literal)
        {
          //This MUST be an identifer
          mStack.Create(*type, tokens[1].second.mRaw);
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
