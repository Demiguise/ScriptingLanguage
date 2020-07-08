#include "executor.h"

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
{}

bool Executor::Execute()
{
  return false;
}
