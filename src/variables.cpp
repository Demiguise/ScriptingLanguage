#include "variables.h"
#include <stdexcept>
#include <new>

Variable::Variable()
  : mName("NAME_NOT_SET")
  , mType(Type::Void)
  , mData(nullptr)
{
}

Variable::Variable(std::string_view name, TType type)
  : mName(name)
  , mType(type)
{
  mInternal.resize(type->SizeOf());
  mData = &(mInternal.front());
  if (mType->Base() == BaseType::String)
  {
    //Construct this string on the stack space allocated to us
    new(mData) std::string();
  }
}

Variable::Variable(std::string_view name, TType type, Byte* pDataBlock)
  : mName(name)
  , mType(type)
  , mData(pDataBlock)
{
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

Result<bool> Variable::Set(const Variable& rhs)
{
  /*
    This feels foul at the moment, but I'm sure we can make it better.
    Should the variable hold the data? Or should the type?
    If it's the latter, what does the variable do?
    How should the types be structued? With templates of an enum type?
    How would a variable store them?
  */
  switch (mType->Base())
  {
    case BaseType::Void:
    {
      return false;
    }
    case BaseType::Int:
    {
      try
      {
        *(int*)mData = *(int*)rhs.mData;
        return true;
      }
      catch (const std::invalid_argument& e)
      {
        return false;
      }
      catch (const std::out_of_range& e)
      {
        return false;
      }
    }
    case BaseType::Bool:
    {
      *(bool*)mData = *(bool*)rhs.mData;
      return true;
    }
    case BaseType::String:
    {
      *(std::string*)mData = *(std::string*)rhs.mData;
      return true;
    }
    case BaseType::Float:
    {
      try
      {
        *(float*)mData = *(float*)rhs.mData;
        return true;
      }
      catch (const std::invalid_argument& e)
      {
        return false;
      }
      catch (const std::out_of_range& e)
      {
        return false;
      }
    }
  }
}
/*
Result<void> Variable::Add(const Variable& rhs)
{
  switch (mType->Base())
  {
    case BaseType::Void:
    {
      return false;
    }
    case BaseType::Int:
    {
      try
      {
        *(int*)mData += *(int*)rhs.mData;
        return true;
      }
      catch (const std::invalid_argument& e)
      {
        return false;
      }
      catch (const std::out_of_range& e)
      {
        return false;
      }
    }
    case BaseType::Bool:
    {
      return false;
    }
    case BaseType::String:
    {
      *(std::string*)mData += *(std::string*)rhs.mData;
      return true;
    }
    case BaseType::Float:
    {
      try
      {
        *(float*)mData += *(float*)rhs.mData;
        return true;
      }
      catch (const std::invalid_argument& e)
      {
        return false;
      }
      catch (const std::out_of_range& e)
      {
        return false;
      }
    }
  }
}
*/

//Templated Stuff
template<class T, T t>
class Variable_Impl : public Variable
{
public:
  virtual Result<bool> Add(const TVar& rhs) override 
  {
    return false;
  }
};

//Template Specialisation
template<>
class Variable_Impl<BaseType, BaseType::Int> : public Variable
{
public:
  virtual Result<bool> Add(const TVar& rhs) override 
  {
    return false;
  }
};

TVar Variable::Create(std::string_view name, TType type, Byte* pDataBlock /*= nullptr*/)
{
  TVar result;
  switch (type->Base())
  {
    default: result = nullptr; break;
  }

  return result;
}
