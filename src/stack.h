#ifndef __STACK_H__
#define __STACK_H__

#include "common.h"
#include "types.h"
#include "variables.h"
#include <deque>
#include <vector>

class Stack
{
public:
  enum class FrameType
  {
    Base,
    Function,
  };

private:
  using TVarVec = std::vector<TVar>;

  struct Frame
  {
    TVarVec mVariables;
    FrameType mType;
    size_t mUsedBytes;

    Frame(FrameType type)
      : mType(type)
      , mUsedBytes(0) {}
  };

  enum class ShadowResult
  {
    Ok,
    Exists,
  };

  using TFrame = std::deque<Frame>;
  using TStack = std::vector<unsigned char>;

  TFrame mFrames;
  TStack mStack;
  TStack::iterator mNext;

  ShadowResult CheckForShadows(std::string_view name);

public:
  Stack(size_t stackSize);

  Result<TVar> Create(Type type, std::string_view name);
  Result<TVar> Get(std::string_view name);

  void EnterFrame(FrameType type);
  void ExitFrame();
};

#endif //__STACK_H__
