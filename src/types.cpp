#include "types.h"

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

Variable::Variable(Type type, std::string_view name)
  : mType(type)
  , mName(name)
{
  //Default set our data to 0.
  mData.i = 0;
}
