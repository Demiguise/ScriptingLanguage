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
  Result<bool> result;
  std::string arg(data);

  if (arg.find(".") != std::string::npos)
  {
    result.AddWarning(Util::Format("(%s) contains an '.' and may lose data when converted", arg.c_str()));
  }

  if (arg.find(",") != std::string::npos)
  {
    result.AddWarning(Util::Format("(%s) contains an ',' and may lose data when converted", arg.c_str()));
  }

  try
  {
    *(int *)mData = std::stoi(arg);
    result.SetResult(true);
  }
  catch (const std::invalid_argument &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Invalid_Argument]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    result.SetError({ VariableError::CannotConvert, errMessage });
  }
  catch (const std::out_of_range &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Out_Of_Range]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    result.SetError({ VariableError::CannotConvert, errMessage });
  }

  return result;
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
  Result<bool> result;
  std::string arg(data);

  if (arg.find(",") != std::string::npos)
  {
    result.AddWarning(Util::Format("(%s) contains an ',' and may lose data when converted", arg.c_str()));
  }

  try
  {
    *(float*)mData = std::stof(arg);
    result.SetResult(true);
  }
  catch (const std::invalid_argument &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Invalid_Argument]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    result.SetError({ VariableError::CannotConvert, errMessage });
  }
  catch (const std::out_of_range &e)
  {
    std::string errMessage = Util::Format("Cannot use (%s) as type [%s][Out_Of_Range]", arg.c_str(), BaseTypeToString(mType->Base()).c_str());
    result.SetError({ VariableError::CannotConvert, errMessage });
  }

  return result;
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

TEST_CASE("Variable::Int::Set", "[Variables]")
{
  TypeRegistry registry;
  TType type = registry.FindType("int");

  std::string varName = "test-int";
  std::vector<Byte> fakeStack;
  fakeStack.resize(type->SizeOf());
  int* pData = (int*)fakeStack.data();

  TVar lhs = Variable::Create(varName, type, fakeStack.data());

  //Basic check to make sure variable names, types,
  //and default initialisation are as expected.
  REQUIRE(lhs->Name() == varName);
  REQUIRE(lhs->VarType()->Base() == BaseType::Int);
  REQUIRE(*pData == 0);

  //Verify ints are set as expected based on string arguments
  Result<bool> testResult;

  testResult = lhs->Set("1");
  REQUIRE(testResult);
  REQUIRE(*pData == 1);

  testResult = lhs->Set("2147483647");
  REQUIRE(testResult);
  REQUIRE(*pData == 2147483647);

  testResult = lhs->Set("-2147483648");
  REQUIRE(testResult);
  REQUIRE(*pData == -2147483648);

  //Verify some values can be converted, but generate a warning
  testResult = lhs->Set("512.12345");
  REQUIRE(testResult);
  REQUIRE(testResult.Warnings().size() != 0);
  REQUIRE(*pData == 512);

  testResult = lhs->Set("1,234,567");
  REQUIRE(testResult);
  REQUIRE(testResult.Warnings().size() != 0);
  REQUIRE(*pData == 1);

  //Verify we can't set int to invalid values
  testResult = lhs->Set("-9999999999");
  REQUIRE(!testResult);
  REQUIRE(testResult.Error().mMessage != "");

  testResult = lhs->Set("muffins");
  REQUIRE(!testResult);
  REQUIRE(testResult.Error().mMessage != "");
}

TEST_CASE("Variable::Float::Set", "[Variables]")
{
  TypeRegistry registry;
  TType type = registry.FindType("float");

  std::string varName = "test-float";
  std::vector<Byte> fakeStack;
  fakeStack.resize(type->SizeOf());
  float* pData = (float*)fakeStack.data();

  TVar lhs = Variable::Create(varName, type, fakeStack.data());

  //Basic check to make sure variable names, types,
  //and default initialisation are as expected.
  REQUIRE(lhs->Name() == varName);
  REQUIRE(lhs->VarType()->Base() == BaseType::Float);
  REQUIRE(*pData == 0.f);

  //Verify float are set as expected based on string arguments
  Result<bool> testResult;

  testResult = lhs->Set("1.5f");
  REQUIRE(testResult);
  REQUIRE(*pData == 1.5f);

  testResult = lhs->Set("3.402823466e+38F");
  REQUIRE(testResult);
  REQUIRE(*pData == 3.402823466e+38F);

  testResult = lhs->Set("1.175494351e-38F");
  REQUIRE(testResult);
  REQUIRE(*pData == 1.175494351e-38F);

  //Verify some values can be converted, but generate a warning
  testResult = lhs->Set("1,234,567");
  REQUIRE(testResult);
  REQUIRE(testResult.Warnings().size() != 0);
  REQUIRE(*pData == 1.f);

  //Verify we can't set int to invalid values
  testResult = lhs->Set("3.402823466e+99F");
  REQUIRE(!testResult);
  REQUIRE(testResult.Error().mMessage != "");

  testResult = lhs->Set("muffins");
  REQUIRE(!testResult);
  REQUIRE(testResult.Error().mMessage != "");
}
#endif
