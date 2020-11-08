#include "types.h"
#include <limits>

namespace util
{
  TType CreateType(BaseType type, std::string_view name)
  {
    return std::make_shared<Type>(type, name);
  }
}

TType Type::Void = util::CreateType(BaseType::Void, "Void");

#define OUTPUT_TYPE(type) case type: return #type
std::string BaseTypeToString(BaseType type)
{
  switch (type)
  {
    OUTPUT_TYPE(BaseType::Void);
    OUTPUT_TYPE(BaseType::Int);
    OUTPUT_TYPE(BaseType::Bool);
    OUTPUT_TYPE(BaseType::String);
    OUTPUT_TYPE(BaseType::Float);
  }
}
#undef OUTPUT_TYPE

Type::Type(BaseType type, std::string typeName)
  : mBase(type)
  , mName(typeName)
{
}

size_t Type::SizeOf()
{
  switch (mBase)
  {
    case BaseType::Int: return sizeof(int);
    case BaseType::Bool: return sizeof(bool);
    case BaseType::String: return sizeof(std::string);
    case BaseType::Float: return sizeof(float);

    case BaseType::Void: 
    default:
      return UINT32_MAX; //This is huge on purpose to TRY and catch errors.
  }
}

TypeRegistry::TypeRegistry()
{
  //Initialize our base types
  mRegistry = {
    { "int" , util::CreateType(BaseType::Int, "int")},
    { "bool" , util::CreateType(BaseType::Bool, "bool")},
    { "string" , util::CreateType(BaseType::String, "string")},
    { "float" , util::CreateType(BaseType::Float, "float")}
  };
}

void TypeRegistry::RegisterTypedef(BaseType base, std::string_view name)
{
  TType existingType = FindType(name);
  if (!existingType->IsVoid())
  {
    //TODO: Bubble up an error here since the type already exists
    return;
  }

  //Put the new type into the registry
  std::string typeName(name);
  mRegistry[typeName] = std::make_shared<Type>(base, typeName);
}

TType TypeRegistry::FindType(std::string_view name)
{
  std::string typeName(name);
  auto iter = mRegistry.find(typeName);
  if (iter == mRegistry.end())
  {
    return Type::Void;
  }

  return iter->second;
}
