#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include "common.h"
#include "tokeniser.h"
#include "stack.h"
#include "types.h"
#include "ASTree.h"

#include <vector>
#include <string>

class Executor
{
private:
  Tokeniser mTokeniser;
  Stack mStack;
  TypeRegistry mRegistry;

  Result<TVar> HandleEquals(std::vector<ASTNode>& children, bool bTopLevel);
  Result<TVar> HandleAddition(std::vector<ASTNode>& children, bool bTopLevel);

  Result<TVar> HandleOperator(TTokenPair op, std::vector<ASTNode>& children, bool bTopLevel);

  TError HandleTokens(TTokenGroup tokens);
  TError ProcessTree(ASTNode& tree);

public:
  Executor();

  Result<bool> SetScript(std::string scriptPath);
  TError Execute();
};

#endif //~__EXECUTOR_H__
