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
                  [&](TVar& ele) { return ele->Name() == name; } );

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

Result<TVar> Stack::Create(Type type, std::string_view name)
{
  if (mFrames.size() == 0)
  {
    return StackError::NoStackFrames;
  }

  if (type.IsNull())
  {
    return StackError::CannotAllocateNull;
  }

  size_t varSize = type.SizeOf();
  if (varSize > (mStack.end() - mNext))
  {
    return StackError::NotEnoughMemory;
  }

  if (CheckForShadows(name))
  {
    std::cout << "An existing variable named [" << name << " already exists." << std::endl;
  }

  std::cout << "Creating new [" << type.Name() << ":" << BaseTypeToString(type.Base()) << "]. Name: " << name << std::endl;
  Frame& topFrame = mFrames.back();

  auto newVar = topFrame.mVariables.emplace_back(name, type, &(*mNext));
  mNext += varSize;
  topFrame.mUsedBytes += varSize;

  return newVar;
}

Result<TVar> Stack::Get(std::string_view name)
{
  if (mFrames.size() == 0)
  {
    return StackError::NoStackFrames;
  }

  for (auto frame : mFrames)
  {
    auto varIter = std::find_if(frame.mVariables.begin(), frame.mVariables.end(), 
                  [&](TVar& ele) { return ele->Name() == name; } );

    if (varIter != frame.mVariables.end())
    {
      return *varIter;
    }

    if (frame.mType == FrameType::Function)
    {
      //We cannot go further back then THIS function call.
      return StackError::VariableDoesNotExist;
    }
  }

  return StackError::VariableDoesNotExist;
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
    mNext -= mFrames.back().mUsedBytes;
    mFrames.pop_back();
    std::cout << "Exited stack frame" << std::endl;
  }
}
