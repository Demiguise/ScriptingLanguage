#include "ASTree.h"
#include "variables.h"

#include <algorithm> 
#include <string_view>

bool IsAVariable(std::string_view arg, Variable& outVar);
bool IsAnIdentifier(std::string_view arg);
bool IsAType(std::string_view arg, Type& outType);
bool IsAKeyword(std::string_view arg);
bool IsABuiltin(std::string_view arg);

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
  return false;
}

bool IsAKeyword(std::string_view arg)
{
  return false;
}

bool IsABuiltin(std::string_view arg)
{
  return false;
}

void ASTNode::BuildTree(TTokenGroup::iterator begin, TTokenGroup::iterator end, ASTNode& parent, TypeRegistry& registry)
{
  if (begin == end)
  {
    //Nothing left to do
    return;
  }

  auto iter = std::find_if(begin, end, [&](TTokenPair& element)
  {
    TType type;
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
  BuildTree(begin, iter, lhs, registry);
  parent.mChildren.push_back(lhs);

  ++iter;

  ASTNode rhs;
  rhs.mTokens.assign(iter, end);
  BuildTree(iter, end, rhs, registry);
  parent.mChildren.push_back(rhs);
}

#ifdef USE_UNIT_TESTS
#include <catch2/catch.hpp>

TEST_CASE("ASTree::Basic", "[ASTree]")
{

}
#endif
