#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <system_error>
#include <optional>
#include <vector>
#include <string>

enum class TokenError 
{
  Success = 0,
  StreamEmpty,
  HandlerError
};

enum class VariableError
{
  Success = 0,
  CannotConvert,
  OperationNotImplemented
};

enum class StackError
{
  Success = 0,
  NoStackFrames,
  CannotAllocateVoid,
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
struct ErrorPair
{
  TError mCode;
  std::string mMessage;
};

TError make_error_code(TokenError);
TError make_error_code(VariableError);
TError make_error_code(StackError);
TError make_error_code(ExecutorError);

template<typename T>
class Result
{
  std::optional<T> mResult;
  std::vector<std::string> mWarnings;
  ErrorPair mErr;

public:
  Result() = default;
  Result(T data)
    : mResult(data) {}
  Result(TError err, std::string message = "")
    : mErr({err, message}) {}

    Result(ErrorPair pair)
      : mErr(pair) {}
  /*
    We SHOULD be able to implicitly convert from StackError to std::error
    but I can't figured out why/how it's not working atm.
  */
  Result(StackError err, std::string message = "")
    : mErr({err, message}) {}

  Result(ExecutorError err, std::string message = "")
    : mErr({err, message}) {}

  Result(VariableError err, std::string message = "")
    : mErr({err, message}) {}

  Result(TokenError err, std::string message = "")
    : mErr({err, message}) {}

  void SetResult(T data) { mResult = data; }
  void ClearResult() { mResult.reset(); }

  ErrorPair Error() { return mErr; }
  void SetError(ErrorPair pair) { mErr = pair; }
  void SetError(TError err, std::string message = "") 
  { 
    mErr.mCode = err; 
    mErr.mMessage = message;
  }

  std::vector<std::string>& Warnings() { return mWarnings; }
  void AddWarning(std::string warning) { mWarnings.push_back(warning); }

  operator bool() const { return mResult.has_value(); }
  T& operator*() { return *mResult; }
};

#endif //~__ERRORS_H__
