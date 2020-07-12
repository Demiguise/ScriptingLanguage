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

bool Executor::IsAVariable(std::string_view arg, Variable& outVar)
{
  return false;
}

bool Executor::IsAnIdentifier(std::string_view arg)
{
  return true;
}

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

bool Executor::HandleTokens(TTokenGroup tokens)
{
  /*
    I REALLY don't like doing this, but I can't visualise a nice way
    of defining the decisions and functions one should take.
    I should probably research what a compiler attempts to do?
  */

  Type type = Type::Null;
  Variable var;

  auto iter = tokens.begin();
  const auto& first = (*iter);
  if (first.first == Token::Literal)
  {
    const std::string_view& raw = first.second.mRaw;
    if (IsAType(raw, type))
    {
      const auto& varName = *(++iter);
      if (varName.first == Token::Literal)
      {
        if (IsAnIdentifier(varName.second.mRaw))
        {
          //We can make a variable from this
          mStack.Create(type, varName.second.mRaw, var);
          const auto& next = *(++iter);
          if (next.first == Token::Statement_End)
          {
            return true;
          }
          else if (next.first == Token::Equals)
          {
            //Handle assignment
            Variable rhsVar;
            const auto& value = *(++iter);
            if (value.first == Token::Literal)
            {
              if (!var.Set(value.second.mRaw))
              {
                return false;
              }
            }
            else if ( value.first == Token::Double_Quote ||
                      value.first == Token::Single_Quote)
            {
              //Handle strings
              const auto& strValue = *(++iter);
              if (strValue.first == Token::Literal)
              {
                var.Set(strValue.second.mRaw);
              }
              else
              {
                //Failure
                return false;
              }
            }
            else if (IsAVariable(value.second.mRaw, rhsVar))
            {
              //Handle assignment
            }
            else
            {
              //Failure
              return false;
            }
          }
          else
          {
            return false;
          }
        }
        else
        {
          //Failure
          return false;
        }
      }
      else
      {
        //Failure
        return false;
      }
    }
    else if (IsAKeyword(raw))
    {
      return false;
    }
    else if (IsABuiltin(raw))
    {
      return false;
    }
    else if (IsAVariable(raw, var))
    {
      return false;
    }
    else
    {
      //Failure
      return false;
    }
  }
  else
  {
    //Failure
    return false;
  }

  return true;
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

    if (!HandleTokens(tokens))
    {
      return false;
    }
  }

  return true;
}

/*
[
  IsAType(token) : [
    IsAnIdentifier(token) : [
      //Check for operators, etc
    ],
    Failure()
  },
  IsAKeyword(token) : {
    ??
  },
  IsABuiltin(token) : {
    FunctionCall(token)
  },
  IsAVariable(token) : {
    IsAnOperator(token, Token::Equals) : {
      //Equals
    },
  }
  Failure()
]
*/
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
