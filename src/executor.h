#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include "tokeniser.h"
#include "stack.h"

class Executor
{
private:
  Tokeniser mTokeniser;
  Stack mStack;

  bool IsAType(std::string_view arg, Type& outType);
  bool IsAKeyword(std::string_view arg);
  bool IsABuiltin(std::string_view arg);

public:
  Executor(std::string scriptPath);
  
  bool Execute();
};

#endif //~__EXECUTOR_H__
