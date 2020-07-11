#include "types.h"

#define OUTPUT_TYPE(type) case type: return #type
std::string TypeToString(Type type)
{
  switch (type)
  {
    OUTPUT_TYPE(Type::Int);
    OUTPUT_TYPE(Type::Bool);
    OUTPUT_TYPE(Type::String);
    OUTPUT_TYPE(Type::Float);
  }
}
#undef OUTPUT_TYPE
