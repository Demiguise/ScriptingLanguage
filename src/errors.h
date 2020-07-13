#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <system_error>
#include <optional>

enum class TokenError
{
  MissingNo,
};

enum class VariableError
{

};

enum class StackError
{

};

enum class ExecutorError
{

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

std::error_code make_error_code(TokenError);
std::error_code make_error_code(VariableError);
std::error_code make_error_code(StackError);
std::error_code make_error_code(ExecutorError);

using TError = std::optional<std::error_code>;

#endif //~__ERRORS_H__
