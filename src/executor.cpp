#include "executor.h"

#include <stdio.h>
#include <iostream>

#include <vector>
#include <algorithm>

#include "types.h"

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
  , mStack(1024 * 16) //16Kib
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
  TTokenGroup tokens;

  mStack.EnterFrame(FrameType::Function);

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

    if (tokens.front().first == Token::Comment)
    {
      //Ignore comments
      continue;
    }
  }

  Variable testVar;
  if (mStack.Create(sTypes[0], "Test", testVar))
  {
    testVar.Set("14");
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
