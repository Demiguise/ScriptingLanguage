#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#include "common.h"
#include "types.h"
#include "errors.h"

#include <string>
#include <array>
#include <memory>

class Variable;
using TVar = std::shared_ptr<Variable>;

//A variable comprises a type, some data, and a name
class Variable
{
private:
  std::string mName;
  TType mType;
  Byte* mData;

  //mInternal is used in the case where the variable is a temporary
  std::vector<Byte> mInternal;

  Variable();
  Variable(std::string_view name, TType type);
  Variable(std::string_view name, TType type, Byte* pDataBlock);
  ~Variable();

public:

  TType VarType() { return mType; }
  std::string Name() { return mName; }
  
  virtual Result<bool> Set(const Variable& rhs);
  virtual Result<bool> Add(const TVar& rhs) = 0;

  static TVar Create(std::string_view name, TType type, Byte* pDataBlock = nullptr);
};


#endif //~__VARIABLES_H__
