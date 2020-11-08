#include "executor.h"
#include "ASTree.h"

#include <stdio.h>
#include <iostream>

#include <vector>
#include <algorithm>

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

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
  , mStack(1024 * 16) //16Kib
{}

TError Executor::HandleTokens(TTokenGroup tokens)
{
  ASTNode tree;

  if (tokens.back().first != Token::Statement_End)
  {
    return ExecutorError::Success;
  }

  ASTNode::BuildTree(tokens.begin(), --(tokens.end()), tree);

  return ProcessTree(tree);
}

TError Executor::HandleEquals(std::vector<ASTNode>& children)
{
  /*
    LHS should include at LEAST an identifier at the end
    type identifer = x;
    identifer = x;
    namespace::type identifier = x;
  */
  ASTNode& LHS = children[0];
  TTokenPair& identifier = LHS.mTokens.back();

  auto result = mStack.Get(identifier.second.mRaw);
  if (!result)
  {
    if (result.Error() != StackError::VariableDoesNotExist)
    {
      //Push error upwards if we can't handle it
      //TODO: Make executor use results as well?
      return result.Error();
    }

    //Try to make it if we can using whatever type information we're given
    if (LHS.mTokens.size() < 2)
    {
      //Can't do anything here, there's no type information
      return ExecutorError::Temp;
    }

    //I know this might break when we get to composite types (namespace::type)
    TTokenPair& typeInfo = LHS.mTokens.front();

    Type& type = mRegistry.FindType(typeInfo.second.mRaw);

    result = mStack.Create(type, identifier.second.mRaw);
    if (!result)
    {
      return result.Error();
    }
  }

  //This should be at LEAST a bit of text or an identifier
  TVar stackVar = *result;
  ASTNode& RHS = children[1];
  TTokenPair& value = RHS.mTokens.front();
  stackVar->Set(value.second.mRaw);

  return ExecutorError::Success;
}

TError Executor::ProcessTree(ASTNode& tree)
{
  switch (tree.mType)
  {
    case ASTNodeType::Operator:
    {
      //This is an operator, which means we should have only 2 children.
      if (tree.mChildren.size() != 2)
      {
        return ExecutorError::MismatchedChildrenSize;
      }

      //TODO: 
      TTokenPair& op = tree.mTokens[0];
      switch (op.first)
      {
        case Token::Equals: return HandleEquals(tree.mChildren);
        default: return ExecutorError::UnknownOperator;
      }
    }
    break;
    case ASTNodeType::Function:
    {}
    break;
    case ASTNodeType::Literal:
    {}
    break;
  }

  return ExecutorError::Success;
}

TError Executor::Execute()
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

    TError err = HandleTokens(tokens);

    if (err)
    {
      return err;
    }
  }

  return ExecutorError::Success;
}

