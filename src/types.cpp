#include "types.h"
#include <stdexcept>

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

bool Variable::Set(const std::string_view& rhs)
{
  /*
    This feels foul at the moment, but I'm sure we can make it better.
    Should the variable hold the data? Or should the type?
    If it's the latter, what does the variable do?
    How should the types be structued? With templates of an enum type?
    How would a variable store them?
  */
  std::string arg(rhs);
  switch (mType.Base())
  {
    case BaseType::Null:
    {
      return false;
    }
    case BaseType::Int:
    {
      try
      {
        mData.i = std::stoi(arg);
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
      if (rhs == "true")
      {
        mData.b = true;
        return true;
      }
      else if (rhs == "false")
      {
        mData.b = false;
        return true;
      }
      else
      {
        return false;
      }
    }
    case BaseType::String:
    {
      mData.str = std::move(arg);
      return false;
    }
    case BaseType::Float:
    {
      try
      {
        mData.f = std::stof(arg);
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

bool Variable::Add(const std::string_view& rhs)
{
  std::string arg(rhs);
  switch (mType.Base())
  {
    case BaseType::Null:
    {
      return false;
    }
    case BaseType::Int:
    {
      try
      {
        mData.i += std::stoi(arg);
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
      mData.str += std::move(arg);
      return false;
    }
    case BaseType::Float:
    {
      try
      {
        mData.f += std::stof(arg);
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
