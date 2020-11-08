#ifndef __TYPES_H__
#define __TYPES_H__

#include "common.h"
#include <string>
#include <unordered_map>

//All other types in our language are defined from these POD types.
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
  Type() = default;
  Type(BaseType base, std::string typeName);

  BaseType Base() { return mBase; }
  std::string Name() { return mName; }
  size_t SizeOf();
  bool IsNull() { return mBase == BaseType::Null; }

  static Type Null;
};

class TypeRegistry
{
private:
  using TRegistry = std::unordered_map<std::string, Type>;
  TRegistry mRegistry;

public:
  TypeRegistry();

  void RegisterTypedef(BaseType base, std::string_view name);
  Type& FindType(std::string_view name);
};

#endif //~__TYPES_H__
