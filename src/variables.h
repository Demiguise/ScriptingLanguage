#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#include "common.h"
#include "types.h"
#include <string>

//A variable comprises a type, some data, and a name
class Variable
{
private:
  std::string mName;
  Type mType;
  Byte* mData;

public:
  Variable();
  Variable(std::string_view name, Type type, Byte* pDataBlock);
  ~Variable();

  Type VarType() { return mType; }
  std::string Name() { return mName; }
  
  bool Set(const std::string_view& rhs);
  bool Add(const std::string_view& rhs);
};

#endif //~__VARIABLES_H__
