#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#include "common.h"
#include "types.h"
#include <string>
#include <array>
#include <memory>

//A variable comprises a type, some data, and a name
class Variable
{
private:
  std::string mName;
  TType mType;
  Byte* mData;

  //mInternal is used in the case where the variable is a temporary
  std::vector<Byte> mInternal;

public:
  Variable();
  Variable(std::string_view name, TType type);
  Variable(std::string_view name, TType type, Byte* pDataBlock);
  ~Variable();

  TType VarType() { return mType; }
  std::string Name() { return mName; }
  
  bool Set(const std::string_view& rhs);
  bool Set(const Variable& rhs);

  bool Add(const std::string_view& rhs);
  bool Add(const Variable& rhs);
};

using TVar = std::shared_ptr<Variable>;

#endif //~__VARIABLES_H__
