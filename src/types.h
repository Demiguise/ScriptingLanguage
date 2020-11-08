#ifndef __TYPES_H__
#define __TYPES_H__

#include "common.h"
#include <string>
#include <unordered_map>

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

class TypeRegistry
{
private:
  using TRegistry = std::unordered_map<std::string, Type>;
  TRegistry mRegistry;

public:
  TypeRegistry() = default;

  void RegisterTypedef(BaseType base, std::string name);
  Type& FindType(std::string_view name);
};

#endif //~__TYPES_H__
