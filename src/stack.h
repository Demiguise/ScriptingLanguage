#ifndef __STACK_H__
#define __STACK_H__

#include "types.h"
#include <stack>
#include <vector>
#include <optional>

using TVarVec = std::vector<Variable>;

struct Frame
{
  TVarVec mVariables;
};

class Stack
{
private:
  using TStackFrame = std::stack<Frame>;
  TStackFrame mFrames;

  bool CheckForShadows(std::string_view name);

public:
  Stack();

  void CreateVariable(Type type, std::string_view name);

  bool GetVariable(std::string_view name, Variable& outVar);

  void EnterFrame();
  void ExitFrame();
};

#endif //__STACK_H__
