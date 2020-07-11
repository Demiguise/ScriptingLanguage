#ifndef __STACK_H__
#define __STACK_H__

#include "types.h"
#include <stack>

class Frame
{

};

class Stack
{
private:
  using TStackFrame = std::stack<Frame>;
  TStackFrame mFrames;

public:
  Stack();

  void CreateVariable(Type type, std::string_view name);

  void EnterFrame();
  void ExitFrame();
};

#endif //__STACK_H__
