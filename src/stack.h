#ifndef __STACK_H__
#define __STACK_H__

#include "types.h"
#include <deque>
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
  using TStackFrame = std::deque<Frame>;
  TStackFrame mFrames;

  bool CheckForShadows(std::string_view name);

public:
  Stack();

  void Create(Type type, std::string_view name);
  bool Get(std::string_view name, Variable& outVar);

  void EnterFrame();
  void ExitFrame();
};

#endif //__STACK_H__
