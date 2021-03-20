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
  for (auto iter = mFrames.rbegin() ; iter != mFrames.rend() ; iter++)
  {
    auto frame = *iter;
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

  for (auto iter = mFrames.rbegin() ; iter != mFrames.rend() ; iter++)
  {
    auto frame = *iter;
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

#ifdef USE_UNIT_TESTS
#include "catch2/catch.hpp"
TEST_CASE("Stack::Basic", "[Stack]")
{
  Stack stack(128);
  TypeRegistry registry;
  stack.EnterFrame(Stack::FrameType::Function);

  auto result = stack.Create(registry.FindType("int"), "a");
  REQUIRE(result);
  REQUIRE(result.Warnings().size() == 0);

  result = stack.Get("a");
  REQUIRE(result);
}

TEST_CASE("Stack::Shadows::Warning", "[Stack]")
{
  Stack stack(128);
  TypeRegistry registry;
  stack.EnterFrame(Stack::FrameType::Function);

  auto result = stack.Create(registry.FindType("int"), "a");
  REQUIRE(result);
  REQUIRE(result.Warnings().size() == 0);

  // Make sure there's a warning here for shadows
  result = stack.Create(registry.FindType("int"), "a");
  REQUIRE(result);
  REQUIRE(result.Warnings().size() != 0);

  // Enter an arbitrary scope change
  stack.EnterFrame(Stack::FrameType::Base);

  // Old variable should still be reachable
  result = stack.Create(registry.FindType("int"), "a");
  REQUIRE(result);
  REQUIRE(result.Warnings().size() != 0);

  result = stack.Get("a");
  REQUIRE(result);
}

TEST_CASE("Stack::Shadows::NewStack", "[Stack]")
{
  Stack stack(128);
  TypeRegistry registry;
  stack.EnterFrame(Stack::FrameType::Function);

  auto result = stack.Create(registry.FindType("int"), "a");
  REQUIRE(result);
  REQUIRE(result.Warnings().size() == 0);

  // Enter a new frame
  stack.EnterFrame(Stack::FrameType::Function);

  // Should be no warning now since it's a completely new stack frame
  result = stack.Create(registry.FindType("int"), "a");
  REQUIRE(result);
  REQUIRE(result.Warnings().size() == 0);

  result = stack.Get("a");
  REQUIRE(result);
}

TEST_CASE("Stack::Memory", "[Stack]")
{
  Stack stack(4);
  TypeRegistry registry;
  stack.EnterFrame(Stack::FrameType::Function);

  auto result = stack.Create(registry.FindType("int"), "a");
  REQUIRE(result);
  REQUIRE(result.Warnings().size() == 0);

  result = stack.Create(registry.FindType("int"), "b");
  REQUIRE(!result);
}
#endif
