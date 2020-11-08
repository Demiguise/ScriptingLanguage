#include "ASTree.h"
#include "variables.h"

#include <algorithm> 
#include <string_view>

bool IsAVariable(std::string_view arg, Variable& outVar);
bool IsAnIdentifier(std::string_view arg);
bool IsAType(std::string_view arg, Type& outType);
bool IsAKeyword(std::string_view arg);
bool IsABuiltin(std::string_view arg);

using TTypeVec = std::vector<Type>;
TTypeVec sTypes = {
  { BaseType::Int,    "int" },
  { BaseType::Bool,   "bool" },
  { BaseType::String, "string" },
  { BaseType::Float,  "float" },
};

bool IsAVariable(std::string_view arg, Variable& outVar)
{
  return false;
}

bool IsAnIdentifier(std::string_view arg)
{
  return true;
}

bool IsAType(std::string_view arg, Type& outType)
{
  auto type = std::find_if(sTypes.begin(), sTypes.end(), 
    [&](Type& element) { return element.Name() == arg; });

  if (type == sTypes.end())
  {
    return false;
  }
  else
  {
    outType = *type;
    return true;
  }
}

bool IsAKeyword(std::string_view arg)
{
  return false;
}

bool IsABuiltin(std::string_view arg)
{
  return false;
}

void ASTNode::BuildTree(TTokenGroup::iterator begin, TTokenGroup::iterator end, ASTNode& parent)
{
  if (begin == end)
  {
    //Nothing left to do
    return;
  }

  auto iter = std::find_if(begin, end, [&](TTokenPair& element)
  {
    if (IsAnOperator(element.first))
    {
      parent.mType = ASTNodeType::Operator;
      return true;
    }
    else if (IsABuiltin(element.second.mRaw))
    {
      parent.mType = ASTNodeType::Function;
      return true;
    }
    else
    {
      return false;
    }
  });

  if (iter == end)
  {
    //Nothing in this area is an operator
    return;
  }
  
  parent.mTokens.push_back(*iter);

  ASTNode lhs;
  lhs.mTokens.assign(begin, iter);
  BuildTree(begin, iter, lhs);
  parent.mChildren.push_back(lhs);

  ++iter;

  ASTNode rhs;
  rhs.mTokens.assign(iter, end);
  BuildTree(iter, end, rhs);
  parent.mChildren.push_back(rhs);
}
