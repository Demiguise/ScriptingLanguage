#include "executor.h"

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
{}

bool Executor::Execute()
{
  std::string raw_statement = "";
  auto tokens = mTokeniser.Parse(raw_statement);
  return false;
}
