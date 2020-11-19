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
Stack::ShadowResult Stack::CheckForShadows(std::string_view name)
{
  for (auto frame : mFrames)
  {
    auto varIter = std::find_if(frame.mVariables.begin(), frame.mVariables.end(), 
                  [&](TVar& ele) { return ele->Name() == name; } );

    if (varIter != frame.mVariables.end())
    {
      //A shadow exists
      return ShadowResult::Exists;
    }

    if (frame.mType == FrameType::Function)
    {
      //We cannot go further back then THIS function call.
      return ShadowResult::Ok;
    }
  }

  return ShadowResult::Ok;
}

Result<TVar> Stack::Create(TType type, std::string_view name)
{
  if (mFrames.size() == 0)
  {
    return StackError::NoStackFrames;
  }

  if (type->IsVoid())
  {
    return StackError::CannotAllocateVoid;
  }

  size_t varSize = type->SizeOf();
  if (varSize > (mStack.end() - mNext))
  {
    return StackError::NotEnoughMemory;
  }

  Result<TVar> result;

  if (CheckForShadows(name) == ShadowResult::Exists)
  {
    result.AddWarning("An existing variable for this name already exists");
  }

  std::cout << "Creating new [" << type->Name() << ":" << BaseTypeToString(type->Base()) << "]. Name: " << name << std::endl;
  Frame& topFrame = mFrames.back();

  TVar newVar = Variable::Create(name, type, &(*mNext));
  topFrame.mVariables.push_back(newVar);
  mNext += varSize;
  topFrame.mUsedBytes += varSize;

  result.SetResult(newVar);

  return result;
}

Result<TVar> Stack::Get(std::string_view name) const
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
