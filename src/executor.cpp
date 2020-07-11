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

bool Executor::IsAType(std::string_view arg, Type& outType)
{
  auto type = std::find_if(sTypes.begin(), sTypes.end(), 
    [&](Type& element) { return element.Name() == arg; });

  if (type == sTypes.end())
  {
    return false;
  }
  else
  {
    outType = *type;
    return true;
  }
}

bool Executor::IsAKeyword(std::string_view arg)
{
  return false;
}

bool Executor::IsABuiltin(std::string_view arg)
{
  return false;
}

bool Executor::Execute()
{
  std::string raw_statement = "";
  TTokenVec tokens;

  mStack.EnterFrame();

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
      auto name = tokens[0].second.mRaw;

      Variable var;
      Type type = sNullType;
      if (IsAType(name, type))
      {
        //We have a valid type
        if (tokens[1].first == Token::Literal)
        {
          //This MUST be an identifer
          mStack.CreateVariable(type, tokens[1].second.mRaw);
        }
      }
      else if (mStack.GetVariable(name, var))
      {
        //We've got a variable matching this.
        if (tokens[1].first == Token::Equals)
        {
          //Got to update this variable
          if (!var.Set(tokens[2].second.mRaw))
          {
            //Error case
          }
        }
      }
    }
  }

  return true;
}

/*
  Let's try and draw out the possible states we have

  [0]                         [1]                         [2]
  Literal   -> IsAType    ->  Literal -> IsAnIdenfier ->  (Optional) Operator
                                      -> Failure

            -> IsAKeyword ->  <Dependant on Keyword>

            -> IsABuiltin ->  Paren_Open -> 
                          ->  Failure
            -> Failure
  
  Operator  -> Failure
*/
