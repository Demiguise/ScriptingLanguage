#ifndef __TYPES_H__
#define __TYPES_H__

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

  struct
  {
    union
    {
      int i;
      bool b;
      float f;
    };

    std::string str;
  } mData;

public:
  Type(BaseType base, std::string typeName);

  BaseType Base() { return mBase; }
  std::string Name() { return mName; }

  bool Set(const std::string_view& rhs);
  bool Add(const std::string_view& rhs);

  bool IsNull() { return mBase == BaseType::Null; }

  static Type Null;
};

using TVariable = std::pair<Type, std::string>;

#endif //~__TYPES_H__
