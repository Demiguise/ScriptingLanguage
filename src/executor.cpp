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
    Type& type = mRegistry.FindType(typeInfo.second.mRaw);

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
        TVar var = *temp;
        stackVar->Set(*var);
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
  auto LHSVar = mStack.Get(LHSIdent.second.mRaw);

  ASTNode& RHS = children[1];
  TTokenPair& RHSIdent = RHS.mTokens.back();
  auto RHSVar = mStack.Get(RHSIdent.second.mRaw);

  Type deducedType;
  //Attempt to deduce the type used here
  if (LHSVar)
  {
    deducedType = (*LHSVar)->VarType();
  }
  else if (RHSVar)
  {
    deducedType = (*RHSVar)->VarType();
  }

  TVar temp = std::make_shared<Variable>("temp", deducedType);

  if (LHSVar)
  {
    temp->Add(*(*LHSVar));
  }
  else
  {
    temp->Add(LHSIdent.second.mRaw);
  }

  if (RHSVar)
  {
    temp->Add(*(*RHSVar));
  }
  else
  {
    temp->Add(RHSIdent.second.mRaw);
  }

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

      TTokenPair& op = tree.mTokens[0];
      return HandleOperator(op, tree.mChildren, true).Error();
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

  mStack.EnterFrame(Stack::FrameType::Function);

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

