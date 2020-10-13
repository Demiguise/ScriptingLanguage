#include "executor.h"
#include "ASTree.h"

#include <stdio.h>
#include <iostream>

#include <vector>
#include <algorithm>

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
  , mStack(1024 * 16) //16Kib
{}

bool Executor::HandleTokens(TTokenGroup tokens)
{
  ASTNode tree;

  if (tokens.back().first != Token::Statement_End)
  {
    return false;
  }

  ASTNode::BuildTree(tokens.begin(), --(tokens.end()), tree);
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
