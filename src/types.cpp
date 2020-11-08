#include "types.h"
#include <limits>

Type Type::Null(BaseType::Null, "Null");

#define OUTPUT_TYPE(type) case type: return #type
std::string BaseTypeToString(BaseType type)
{
  switch (type)
  {
    OUTPUT_TYPE(BaseType::Null);
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

    case BaseType::Null: 
    default:
      return UINT32_MAX;
  }
}

Type& TypeRegistry::FindType(std::string_view name)
{
  //Forced int for now
  static Type intType{ BaseType::Int, "Int" };
  return intType;
}
