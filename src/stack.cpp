#include "stack.h"

#include <iostream>
#include <stdio.h>

Stack::Stack()
{}

void Stack::CreateVariable(Type type, std::string_view name)
{
  std::cout << "Creating new [" << type.Name() << ":" << BaseTypeToString(type.Base()) << "]. Name: " << name << std::endl;
}

void Stack::EnterFrame()
{
  mFrames.push(Frame());
  std::cout << "Entered new stack frame" << std::endl;
}

void Stack::ExitFrame()
{
  if (mFrames.size() > 0)
  {
    mFrames.pop();
    std::cout << "Exited stack frame" << std::endl;
  }
}
