#ifndef __TYPES_H__
#define __TYPES_H__

#include "common.h"
#include <string>
#include <unordered_map>
#include <memory>

//All other types in our language are defined from these POD types.
enum class BaseType
{
  Void,
  Int,
  Bool,
  String,
  Float,
};
std::string BaseTypeToString(BaseType type);

//Need to forward declare here of the static TType
class Type;
using TType = std::shared_ptr<Type>;

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
  bool IsVoid() { return mBase == BaseType::Void; }

  static TType Void;
};

class TypeRegistry
{
private:
  using TRegistry = std::unordered_map<std::string, TType>;
  TRegistry mRegistry;

public:
  TypeRegistry();

  void RegisterTypedef(BaseType base, std::string_view name);
  TType FindType(std::string_view name);
};

#endif //~__TYPES_H__
