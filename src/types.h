#ifndef __TYPES_H__
#define __TYPES_H__

#include "common.h"
#include <string>

//Temporary while we are dealing with JUST POD types
enum class BaseType
{
  Null,
  Int,
  Bool,
  String,
  Float,
};
std::string BaseTypeToString(BaseType type);

class Type
{
private:
  BaseType mBase;
  std::string mName;

public:
  Type(BaseType base, std::string typeName);

  BaseType Base() { return mBase; }
  std::string Name() { return mName; }
  size_t SizeOf();
  bool IsNull() { return mBase == BaseType::Null; }

  static Type Null;
};

#endif //~__TYPES_H__
