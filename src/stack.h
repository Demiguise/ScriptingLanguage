#ifndef __STACK_H__
#define __STACK_H__

#include "types.h"

class Stack
{
public:
  Stack();

  void Create(Type type, std::string_view name);
};

#endif //__STACK_H__
