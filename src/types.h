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

class Variable
{
private:
  BaseType mBase = BaseType::Null; //Refers to how the data is stored in the union

  union
  {
    int i;
    bool b;
    float f;
    std::string str;
  } mData;

public:
  Variable() = default;
  ~Variable() = default;
};

#endif //~__TYPES_H__
