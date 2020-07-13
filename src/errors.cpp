#include "errors.h"

namespace
{
  struct TokenErrorCategory : std::error_category
  {
    const char* name() const noexcept override
    {
      return "token";
    }

    std::string message(int err) const override
    {
      switch (static_cast<TokenError>(err))
      {
        default:
          return "Unknown error";
      }
    }
  };

  struct VariableErrorCategory : std::error_category
  {
    const char* name() const noexcept override
    {
      return "variable";
    }

    std::string message(int err) const override
    {
      switch (static_cast<VariableError>(err))
      {
        default:
          return "Unknown error";
      }
    }
  };

  struct StackErrorCategory : std::error_category
  {
    const char* name() const noexcept override
    {
      return "stack";
    }

    std::string message(int err) const override
    {
      switch (static_cast<StackError>(err))
      {
        case StackError::NoStackFrames:
          return "No stack frames are currently in use";
        case StackError::CannotAllocateNull:
          return "Cannot allocate memory on the stack for a NULL base type";
        case StackError::NotEnoughMemory:
          return "Cannot allocate enough memory for new variable";
        case StackError::VariableDoesNotExist:
          return "The requested variable does not exist";
        default:
          return "Unknown error";
      }
    }
  };

  struct ExecutorErrorCategory : std::error_category
  {
    const char* name() const noexcept override
    {
      return "executor";
    }

    std::string message(int err) const override
    {
      switch (static_cast<ExecutorError>(err))
      {
        default:
          return "Unknown error";
      }
    }
  };

  const TokenErrorCategory tokenErrorCategory;
  const VariableErrorCategory variableErrorCategory;
  const StackErrorCategory stackErrorCategory;
  const ExecutorErrorCategory executorErrorCategory;
}

std::error_code make_error_code(TokenError err)
{
  return {static_cast<int>(err), tokenErrorCategory};
}

std::error_code make_error_code(VariableError err)
{
  return {static_cast<int>(err), variableErrorCategory};
}

std::error_code make_error_code(StackError err)
{
  return {static_cast<int>(err), stackErrorCategory};
}

std::error_code make_error_code(ExecutorError err)
{
  return {static_cast<int>(err), executorErrorCategory};
}
