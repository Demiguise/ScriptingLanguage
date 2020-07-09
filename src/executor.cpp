#include "executor.h"

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
{}

bool Executor::Execute()
{
  std::string raw_statement = "";
  TTokenVec tokens;
  while (!mTokeniser.Parse(raw_statement, tokens).has_value());
  {
  }

  return true;
}
