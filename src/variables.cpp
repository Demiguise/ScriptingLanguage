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
    new(mData) std::string();
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

/////////////// Int
template<>
Result<bool> Variable_Impl<BaseType, BaseType::Int>::Set(const TVar& rhs)
{
  BaseType type = rhs->mType->Base();
  switch (type)
  {
    case BaseType::Int:
    {
      auto* lhsData = Convert::ToInt(this->mData);
      auto* rhsData = Convert::ToInt(rhs->mData);

      *lhsData = *rhsData;
      return true;
    }
    default: 
    {
      std::string errMessage = Util::Format("Cannot convert [%s] to type [%s]", BaseTypeToString(type).c_str(), BaseTypeToString(mType->Base()).c_str());
      return { VariableError::CannotConvert, errMessage };
    }
  }
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::Int>::Set(const std::string_view& data)
{
  std::string arg(data);
  try
  {
    *(int *)mData = std::stoi(arg);
    return true;
  }
  catch (const std::invalid_argument &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Invalid_Argument]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    return { VariableError::CannotConvert, errMessage };
  }
  catch (const std::out_of_range &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Out_Of_Range]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    return { VariableError::CannotConvert, errMessage };
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
      auto* lhsData = Convert::ToInt(this->mData);
      auto* rhsData = Convert::ToInt(rhs->mData);

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

/////////////// Bool
template<>
Result<bool> Variable_Impl<BaseType, BaseType::Bool>::Set(const TVar& rhs)
{
  BaseType type = rhs->mType->Base();
  switch (type)
  {
    case BaseType::Bool:
    {
      auto* lhsData = Convert::ToBool(this->mData);
      auto* rhsData = Convert::ToBool(rhs->mData);

      *lhsData = *rhsData;
      return true;
    }
    default: 
    {
      std::string errMessage = Util::Format("Cannot convert [%s] to type [%s]", BaseTypeToString(type).c_str(), BaseTypeToString(mType->Base()).c_str());
      return { VariableError::CannotConvert, errMessage };
    }
  }
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::Bool>::Set(const std::string_view& data)
{
  std::string arg(data);
  if (arg == "true")
  {
    *(bool *)mData = true;
    return true;
  }
  else if (arg == "false")
  {
    *(bool *)mData = false;
    return true;
  }
  else
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Invalid_Argument]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    return { VariableError::CannotConvert, errMessage };
  }
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::Bool>::Add(const TVar& rhs)
{
  std::string errMessage = Util::Format("Cannot add to type [%s]", BaseTypeToString(mType->Base()).c_str());
  return { VariableError::CannotConvert, errMessage };
}

/////////////// String
template<>
Result<bool> Variable_Impl<BaseType, BaseType::String>::Set(const TVar& rhs)
{
  BaseType type = rhs->mType->Base();
  switch (type)
  {
    case BaseType::String:
    {
      auto* lhsData = Convert::ToString(this->mData);
      auto* rhsData = Convert::ToString(rhs->mData);

      *lhsData = *rhsData;
      return true;
    }
    default: 
    {
      std::string errMessage = Util::Format("Cannot convert [%s] to type [%s]", BaseTypeToString(type).c_str(), BaseTypeToString(mType->Base()).c_str());
      return { VariableError::CannotConvert, errMessage };
    }
  }
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::String>::Set(const std::string_view& data)
{
  std::string arg(data);
  *(std::string*)mData = std::move(arg);
  return true;
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::String>::Add(const TVar& rhs)
{
  BaseType type = rhs->mType->Base();
  switch (type)
  {
    case BaseType::String:
    {
      auto* lhsData = Convert::ToString(this->mData);
      auto* rhsData = Convert::ToString(rhs->mData);

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

/////////////// Float
template<>
Result<bool> Variable_Impl<BaseType, BaseType::Float>::Set(const TVar& rhs)
{
  BaseType type = rhs->mType->Base();
  switch (type)
  {
    case BaseType::Float:
    {
      auto* lhsData = Convert::ToFloat(this->mData);
      auto* rhsData = Convert::ToFloat(rhs->mData);

      *lhsData = *rhsData;
      return true;
    }
    default: 
    {
      std::string errMessage = Util::Format("Cannot convert [%s] to type [%s]", BaseTypeToString(type).c_str(), BaseTypeToString(mType->Base()).c_str());
      return { VariableError::CannotConvert, errMessage };
    }
  }
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::Float>::Set(const std::string_view& data)
{
  std::string arg(data);
  try
  {
    *(float*)mData = std::stof(arg);
    return true;
  }
  catch (const std::invalid_argument &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Invalid_Argument]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    return { VariableError::CannotConvert, errMessage };
  }
  catch (const std::out_of_range &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Out_Of_Range]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    return { VariableError::CannotConvert, errMessage };
  }
}

template<>
Result<bool> Variable_Impl<BaseType, BaseType::Float>::Add(const TVar& rhs)
{
  BaseType type = rhs->mType->Base();
  switch (type)
  {
    case BaseType::Float:
    {
      auto* lhsData = Convert::ToFloat(this->mData);
      auto* rhsData = Convert::ToFloat(rhs->mData);

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
  if (!type)
  {
    return nullptr;
  }

  TVar result;
  switch (type->Base())
  {
    case BaseType::Int: result = std::make_shared<Variable_Impl<BaseType, BaseType::Int>>(name, type, pDataBlock); break;
    case BaseType::Bool: result = std::make_shared<Variable_Impl<BaseType, BaseType::Bool>>(name, type, pDataBlock); break;
    case BaseType::String: result = std::make_shared<Variable_Impl<BaseType, BaseType::String>>(name, type, pDataBlock); break;
    case BaseType::Float: result = std::make_shared<Variable_Impl<BaseType, BaseType::Float>>(name, type, pDataBlock); break;
    default: result = nullptr; break;
  }

  return result;
}

#ifdef USE_UNIT_TESTS
#include <catch2/catch.hpp>
TEST_CASE("Variable::Create", "[Variables]")
{
  TypeRegistry registry;

  REQUIRE(Variable::Create("test", registry.FindType("int")) != nullptr);
  REQUIRE(Variable::Create("test", registry.FindType("bool")) != nullptr);
  REQUIRE(Variable::Create("test", registry.FindType("string")) != nullptr);
  REQUIRE(Variable::Create("test", registry.FindType("float")) != nullptr);

  REQUIRE(Variable::Create("test", nullptr) == nullptr);
  REQUIRE(Variable::Create("test", Type::Void) == nullptr);
}
#endif
