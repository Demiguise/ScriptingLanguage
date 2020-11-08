#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <system_error>
#include <optional>

enum class TokenError 
{
  Success = 0,
};

enum class VariableError
{
  Success = 0,
};

enum class StackError
{
  Success = 0,
  NoStackFrames,
  CannotAllocateNull,
  NotEnoughMemory,
  VariableDoesNotExist,
};

enum class ExecutorError
{
  Success = 0,
  MismatchedChildrenSize,
  UnknownOperator,
  Temp,
};

namespace std
{
  template<>
    struct is_error_code_enum<TokenError> : true_type {};
  template<>
    struct is_error_code_enum<VariableError> : true_type {};
  template<>
    struct is_error_code_enum<StackError> : true_type {};
  template<>
    struct is_error_code_enum<ExecutorError> : true_type {};
}

using TError = std::error_code;

TError make_error_code(TokenError);
TError make_error_code(VariableError);
TError make_error_code(StackError);
TError make_error_code(ExecutorError);

template<typename T>
class Result
{
  std::optional<T> mResult;
  TError mErr;

public:
  Result(T data)
    : mResult(data) {}
  Result(TError err)
    : mErr(err) {}

  /*
    We SHOULD be able to implicitly convert from StackError to std::error
    but I can't figured out why/how it's not working atm.
  */
  Result(StackError err)
    : mErr(err) {}

  TError Error() { return mErr; }
  operator bool() { return mResult.has_value(); }
  T operator*() { return *mResult; }
};

#endif //~__ERRORS_H__
