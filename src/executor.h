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

  bool HandleTokens(TTokenGroup tokens);

public:
  Executor(std::string scriptPath);
  
  bool Execute();
};

#endif //~__EXECUTOR_H__
