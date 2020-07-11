#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>

//Temporary while we are dealing with JUST POD types
enum class Type
{
  Int,
  Bool,
  String,
  Float,
};

std::string TypeToString(Type type);

#endif //~__TYPES_H__
