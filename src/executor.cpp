#include "executor.h"

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
{}

bool Executor::Execute()
{
  std::string raw_statement = "";
  TTokenVec tokens;
  auto err = mTokeniser.Parse(raw_statement, tokens);

  if (err.has_value())
  {
    return false;
  }

  return true;
}
