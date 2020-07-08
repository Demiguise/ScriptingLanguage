#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include "tokeniser.h"
#include "stack.h"

class Executor
{
private:
  Tokeniser mTokeniser;
  Stack mStack;

public:
  Executor(std::string scriptPath);
  
  bool Execute();
};

#endif //~__EXECUTOR_H__
