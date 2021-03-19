#include "ASTree.h"
#include "variables.h"
#include "keywords.h"

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
    else if (IsKeyword(element.second.mRaw) != Keyword::Null)
    {
      parent.mType = ASTNodeType::Keyword;
      return true;
    }
    else
    {
      return false;
    }
  });

  if (iter == end)
  {
    //Nothing in this area is interesting
    return;
  }

  switch (parent.mType)
  {
    case ASTNodeType::Keyword:
    {
      //Place the keyword into the parent node
      parent.mTokens.push_back(*iter);
      ++iter;

      ASTNode keywordChild;
      BuildTree(iter, end, keywordChild, registry);
      parent.mChildren.push_back(keywordChild);
    }
    break;
    case ASTNodeType::Operator:
    {
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
    break;
    default:
      //Error?
    break;
  }
}

#ifdef USE_UNIT_TESTS
#include <catch2/catch.hpp>

TEST_CASE("ASTree::Basic", "[ASTree]")
{
  TypeRegistry registry;
  ASTNode root;

  std::string outStatement;

  Tokeniser tokeniser;
  tokeniser.SetStream("a = b");
  auto result = tokeniser.Parse(outStatement);
  REQUIRE(result);

  TTokenGroup testGroup = *result;

  ASTNode::BuildTree(testGroup.begin(), testGroup.end(), root, registry);
  REQUIRE(root.mChildren.size() == 2);
  REQUIRE(root.mType == ASTNodeType::Operator);
}

TEST_CASE("ASTree::MultipleOperators", "[ASTree]")
{
  TypeRegistry registry;
  ASTNode root;
  std::string outStatement;

  Tokeniser tokeniser;
  tokeniser.SetStream("a = b + c");
  auto result = tokeniser.Parse(outStatement);
  REQUIRE(result);

  TTokenGroup testGroup = *result;

  ASTNode::BuildTree(testGroup.begin(), testGroup.end(), root, registry);
  REQUIRE(root.mChildren.size() == 2);
  REQUIRE(root.mType == ASTNodeType::Operator);

  auto& rhs = root.mChildren[1];
  REQUIRE(rhs.mChildren.size() == 2);
  REQUIRE(rhs.mType == ASTNodeType::Operator);
}

TEST_CASE("ASTree::Keyword::Using", "[ASTree]")
{
  TypeRegistry registry;
  ASTNode root;
  std::string outStatement;

  Tokeniser tokeniser;
  tokeniser.SetStream("using bob = int;");
  auto result = tokeniser.Parse(outStatement);
  REQUIRE(result);

  TTokenGroup testGroup = *result;

  ASTNode::BuildTree(testGroup.begin(), testGroup.end(), root, registry);
  REQUIRE(root.mChildren.size() == 0);
  REQUIRE(root.mType == ASTNodeType::Keyword);
}

#endif
