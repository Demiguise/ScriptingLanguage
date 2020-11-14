#include "variables.h"
#include <stdexcept>
#include <new>

Variable::Variable()
  : mName("NAME_NOT_SET")
  , mType(Type::Void)
  , mData(nullptr)
{
}

Variable::Variable(std::string_view name, TType type, Byte* pDataBlock)
  : mName(name)
  , mType(type)
  , mData(pDataBlock)
{
  if (pDataBlock == nullptr)
  {
    mInternal.resize(type->SizeOf());
    mData = &(mInternal.front());
  }

  if (mType->Base() == BaseType::String)
  {
    //Construct this string on the stack space allocated to us
    new(pDataBlock) std::string();
  }
}

Variable::~Variable()
{
  if (mType->Base() == BaseType::String)
  {
    //Explicitly call the destructor of the string
    std::string* pStr = (std::string*)mData;
    pStr->~basic_string();
  }
}

//Templated Stuff
template<class T, T t>
class Variable_Impl : public Variable
{
public:
  Variable_Impl(std::string_view name, TType type, Byte* pDataBlock)
    : Variable(name, type, pDataBlock) {}

  virtual Result<bool> Set(const TVar& rhs) override 
  {
    std::string errMessage = Util::Format("No Set operator implemented for [%s]", BaseTypeToString(mType->Base()).c_str());
    return { VariableError::OperationNotImplemented, errMessage };
  }

  virtual Result<bool> Set(const std::string_view& data) override 
  {
    std::string errMessage = Util::Format("No Set operator implemented for [%s]", BaseTypeToString(mType->Base()).c_str());
    return { VariableError::OperationNotImplemented, errMessage };
  }

  virtual Result<bool> Add(const TVar& rhs) override 
  {
    std::string errMessage = Util::Format("No Add operator implemented for [%s]", BaseTypeToString(mType->Base()).c_str());
    return { VariableError::OperationNotImplemented, errMessage };
  }
};

namespace Convert
{
  int* ToInt(Byte* pFrom) 
  {
    return (int*)pFrom;
  }

  float* ToFloat(Byte* pFrom)
  {
    return (float*)pFrom;
  }

  bool* ToBool(Byte* pFrom)
  {
    return (bool*)pFrom;
  }

  std::string* ToString(Byte* pFrom)
  {
    return (std::string*)pFrom;
  }
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::Int>::Add(const TVar& rhs)
{
  BaseType type = rhs->mType->Base();
  switch (type)
  {
    case BaseType::Int:
    {
      int* lhsData = Convert::ToInt(this->mData);
      int* rhsData = Convert::ToInt(rhs->mData);

      *lhsData += *rhsData;
      return true;
    }
    default: 
    {
      std::string errMessage = Util::Format("Cannot convert [%s] to type [%s]", BaseTypeToString(type).c_str(), BaseTypeToString(mType->Base()).c_str());

      return { VariableError::CannotConvert, errMessage };
    }
  }
}

TVar Variable::Create(std::string_view name, TType type, Byte* pDataBlock /*= nullptr*/)
{
  TVar result;
  switch (type->Base())
  {
    case BaseType::Int: result = std::make_shared<Variable_Impl<BaseType, BaseType::Int>>(name, type, pDataBlock); break;
    case BaseType::Bool: result = std::make_shared<Variable_Impl<BaseType, BaseType::Bool>>(name, type, pDataBlock); break;
    default: result = nullptr; break;
  }

  return result;
}
