#include "stack.h"

#include <iostream>
#include <stdio.h>
#include <algorithm>

Stack::Stack()
{}

//False on no shadows existing
//Shadows being other variables named the same thing.
bool Stack::CheckForShadows(std::string_view name)
{
  return false;
}

void Stack::Create(Type type, std::string_view name)
{
  if (mFrames.size() == 0)
  {
    //Error?
    std::cout << "No stack frames in use" << std::endl;
    return;
  }

  std::cout << "Creating new [" << type.Name() << ":" << BaseTypeToString(type.Base()) << "]. Name: " << name << std::endl;
  Frame& topFrame = mFrames.back();
  Variable newVar(type, name);
  topFrame.mVariables.push_back(newVar);

  if (CheckForShadows(name))
  {
    std::cout << "An existing variable named [" << name << " already exists." << std::endl;
  }
}

bool Stack::Get(std::string_view name, Variable& outVar)
{
  if (mFrames.size() == 0)
  {
    //Error
    std::cout << "No stack frames in use" << std::endl;
    return false;
  }

  Frame& topFrame = mFrames.back();
  auto varIter = std::find_if(topFrame.mVariables.begin(), topFrame.mVariables.end(), 
                [&](Variable& ele) { return ele.Name() == name; } );

  if (varIter == topFrame.mVariables.end())
  {
    return false;
  }
  else
  {
    outVar = *varIter;
    return true;
  }
}

void Stack::EnterFrame()
{
  mFrames.push_back(Frame());
  std::cout << "Entered new stack frame" << std::endl;
}

void Stack::ExitFrame()
{
  if (mFrames.size() > 0)
  {
    mFrames.pop_back();
    std::cout << "Exited stack frame" << std::endl;
  }
}
