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

Executor::Executor()
  : mTokeniser()
  , mStack(1024 * 16) //16Kib
{}

Result<bool> Executor::SetScript(std::string scriptPath)
{
  mTokeniser.SetStream("");
  return false;
}

Result<bool> Executor::HandleTokens(TTokenGroup tokens)
{
  ASTNode tree;

  if (tokens.back().first != Token::Statement_End)
  {
    return ExecutorError::Success;
  }

  ASTNode::BuildTree(tokens.begin(), --(tokens.end()), tree, mRegistry);

  return ProcessTree(tree);
}

Result<TVar> Executor::HandleEquals(std::vector<ASTNode>& children, bool bTopLevel)
{
  /*
    LHS should include at LEAST an identifier at the end
    type identifer = x;
    identifer = x;
    namespace::type identifier = x;
  */

  TVar stackVar;
  Result<TVar> result;

  ASTNode& LHS = children[0];

  if (LHS.mTokens.size() > 1)
  {
    //Should contain some kind of type information
    auto iter = LHS.mTokens.rbegin();

    TTokenPair& identifier = (*iter);
    iter++;
    TTokenPair& typeInfo = (*iter);
    TType type = mRegistry.FindType(typeInfo.second.mRaw);

    result = mStack.Create(type, identifier.second.mRaw);
  }
  else if (LHS.mTokens.size() == 1)
  {
    //Should just be an identifier
    TTokenPair& identifier = LHS.mTokens.back();
    result = mStack.Get(identifier.second.mRaw);
  }
  else
  {
    //Nothing to do
    return ExecutorError::Temp;
  }

  if (!result)
  {
    return result;
  }

  stackVar = *result;

  //This should be at LEAST a bit of text or an identifier
  ASTNode& RHS = children[1];
  switch (RHS.mType)
  {
    case ASTNodeType::Operator:
      {
        TTokenPair& op = RHS.mTokens.back();
        auto temp = HandleOperator(op, RHS.mChildren, false);
        if (!temp)
        {
          return temp;
        }

        //TODO: Evaluate whether to have different functiosn for add/subtract/etc.
        auto setResult = stackVar->Set(*temp);
        if (!setResult)
        {
          return setResult.Error();
        }
      }
      break;
    default:
      {
        TTokenPair& value = RHS.mTokens.front();
        stackVar->Set(value.second.mRaw);
      }
      break;
  }

  return stackVar;
}

Result<TVar> Executor::HandleAddition(std::vector<ASTNode>& children, bool bTopLevel)
{
  /*
    LHS should be an identifier for a stack variable, or a literal.
    RHS should be an identifier for a stack variable, or a literal.
  */
  ASTNode& LHS = children[0];
  TTokenPair& LHSIdent = LHS.mTokens.back();
  auto LHSResult = mStack.Get(LHSIdent.second.mRaw);

  ASTNode& RHS = children[1];
  TTokenPair& RHSIdent = RHS.mTokens.back();
  auto RHSResult = mStack.Get(RHSIdent.second.mRaw);

  TType deducedType;
  //Attempt to deduce the type used here
  if (LHSResult)
  {
    deducedType = (*LHSResult)->VarType();
  }
  else if (RHSResult)
  {
    deducedType = (*RHSResult)->VarType();
  }

  //TODO: Better type deduction
  if (deducedType == nullptr)
  {
    deducedType = mRegistry.FindType("int");
  }

  TVar temp = Variable::Create("Addition-Temp", deducedType);

  TVar LHSVar;
  if (!LHSResult)
  {
    LHSVar = Variable::Create("Addition-LHS", deducedType);
    auto result = LHSVar->Set(LHSIdent.second.mRaw);
    if (!result)
    {
      return result.Error();
    }
  }
  else
  {
    LHSVar = *LHSResult;
  }
  temp->Add(LHSVar);

  TVar RHSVar;
  if (!RHSResult)
  {
    RHSVar = Variable::Create("Addition-RHS", deducedType);
    auto result = RHSVar->Set(RHSIdent.second.mRaw);
    if (!result)
    {
      return result.Error();
    }
  }
  else
  {
    RHSVar = *RHSResult;
  }
  temp->Add(RHSVar);

  return temp;
}

Result<TVar> Executor::HandleOperator(TTokenPair op, std::vector<ASTNode>& children, bool bTopLevel)
{
    switch (op.first)
    {
      case Token::Equals: return HandleEquals(children, bTopLevel);
      case Token::Addition: return HandleAddition(children, bTopLevel);
      default: return ExecutorError::UnknownOperator;
    }
}

Result<bool> Executor::ProcessTree(ASTNode& tree)
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

      TTokenPair& op = tree.mTokens[0];
      return HandleOperator(op, tree.mChildren, true).Error().mCode;
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

Result<bool> Executor::Execute()
{
  std::string raw_statement = "";
  TTokenGroup tokens;

  mStack.EnterFrame(Stack::FrameType::Function);

  Result<TTokenGroup> result;
  while ((result = mTokeniser.Parse(raw_statement)))
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

    Result<bool> tokenResult = HandleTokens(tokens);
    if (!tokenResult)
    {
      return tokenResult;
    }
  }

  if (!result)
  {
    return result.Error();
  }

  return ExecutorError::Success;
}

#ifdef USE_UNIT_TESTS
#include "catch2/catch.hpp"
TEST_CASE("TestME", "[base]")
{
  REQUIRE(1 == 1);
}
#endif
