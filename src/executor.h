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

  Result<bool> HandleTokens(TTokenGroup tokens);
  Result<bool> ProcessTree(ASTNode& tree);

public:
  Executor();

  const Stack& GetStack() { return mStack; }
  TypeRegistry& GetRegistry() { return mRegistry; }

  Result<bool> SetScript(std::string scriptString); //Entire buffer in string should be the script
  Result<bool> Execute();
};

#endif //~__EXECUTOR_H__
