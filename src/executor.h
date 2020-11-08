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

  TError HandleEquals(std::vector<ASTNode>& children);

  TError HandleTokens(TTokenGroup tokens);
  TError ProcessTree(ASTNode& tree);

public:
  Executor(std::string scriptPath);
  
  TError Execute();
};

#endif //~__EXECUTOR_H__
