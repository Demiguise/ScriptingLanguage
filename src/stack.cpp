#include "stack.h"

#include <iostream>
#include <stdio.h>
#include <algorithm>

Stack::Stack(size_t stackSize)
{
  mStack.resize(stackSize);
  mNext = mStack.begin();
}

//False on no shadows existing
//Shadows being other variables named the same thing.
bool Stack::CheckForShadows(std::string_view name)
{
  for (auto frame : mFrames)
  {
    auto varIter = std::find_if(frame.mVariables.begin(), frame.mVariables.end(), 
                  [&](Variable& ele) { return ele.Name() == name; } );

    if (varIter != frame.mVariables.end())
    {
      //A shadow exists
      return true;
    }

    if (frame.mType == FrameType::Function)
    {
      //We cannot go further back then THIS function call.
      return false;
    }
  }

  return false;
}

bool Stack::Create(Type type, std::string_view name, Variable& outVar)
{
  if (mFrames.size() == 0)
  {
    //Error?
    std::cout << "No stack frames in use" << std::endl;
    return false;
  }

  if (type.IsNull())
  {
    //Error?
    std::cout << "Cannot allocate a Null type for [" << name << "]" << std::endl;
    return false;
  }

  size_t varSize = type.SizeOf();
  if (varSize > (mStack.end() - mNext))
  {
    //Error?
    std::cout << "Not enough memory available to allocate (" << varSize << ") bytes." << std::endl;
    return false;
  }

  if (CheckForShadows(name))
  {
    std::cout << "An existing variable named [" << name << " already exists." << std::endl;
  }

  std::cout << "Creating new [" << type.Name() << ":" << BaseTypeToString(type.Base()) << "]. Name: " << name << std::endl;
  Frame& topFrame = mFrames.back();

  outVar = topFrame.mVariables.emplace_back(name, type, &(*mNext));
  mNext += varSize;
  topFrame.mUsedBytes += varSize;

  return true;
}

bool Stack::Get(std::string_view name, Variable& outVar)
{
  if (mFrames.size() == 0)
  {
    //Error
    std::cout << "No stack frames in use" << std::endl;
    return false;
  }

  for (auto frame : mFrames)
  {
    auto varIter = std::find_if(frame.mVariables.begin(), frame.mVariables.end(), 
                  [&](Variable& ele) { return ele.Name() == name; } );

    if (varIter != frame.mVariables.end())
    {
      outVar = *varIter;
      return true;
    }

    if (frame.mType == FrameType::Function)
    {
      //We cannot go further back then THIS function call.
      return false;
    }
  }

  return false;
}

void Stack::EnterFrame(FrameType type)
{
  mFrames.push_back(Frame(type));
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
