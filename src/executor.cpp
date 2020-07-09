#include "executor.h"

#include <stdio.h>
#include <iostream>

Executor::Executor(std::string scriptPath)
  : mTokeniser(scriptPath)
{}

bool Executor::Execute()
{
  std::string raw_statement = "";
  TTokenVec tokens;
  while (!mTokeniser.Parse(raw_statement, tokens).has_value())
  {
    std::cout << "RAW: " << raw_statement << std::endl;
    std::cout << "[";
    bool bFirst = true;
    for (const auto& [type, token] : tokens)
    {
      if (!bFirst)
      {
        std::cout << ",";
      }

      std::cout << token.mRaw;
      bFirst = false;
    }
    std::cout << "]" << std::endl << std::flush;
  }

  return true;
}
