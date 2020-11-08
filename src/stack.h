#ifndef __STACK_H__
#define __STACK_H__

#include "common.h"
#include "types.h"
#include "variables.h"
#include <deque>
#include <vector>
#include <optional>
#include <memory>

using TVar = std::shared_ptr<Variable>;
using TVarVec = std::vector<TVar>;

enum class FrameType
{
  Base,
  Function,
};

struct Frame
{
  TVarVec mVariables;
  FrameType mType;
  size_t mUsedBytes;

  Frame(FrameType type)
    : mType(type)
    , mUsedBytes(0) {}
};

class Stack
{
private:
  using TFrame = std::deque<Frame>;
  using TStack = std::vector<unsigned char>;

  TFrame mFrames;
  TStack mStack;
  TStack::iterator mNext;

  bool CheckForShadows(std::string_view name);

public:
  Stack(size_t stackSize);

  Result<TVar> Create(Type type, std::string_view name);
  Result<TVar> Get(std::string_view name);

  void EnterFrame(FrameType type);
  void ExitFrame();
};

#endif //__STACK_H__
