#ifndef __ASTREE_H__
#define __ASTREE_H__

#include "tokeniser.h"
#include "types.h"
#include <vector>

enum class ASTNodeType
{
  Operator, //2 children, LHS and RHS.
  Keyword,  //N Children
  Function, //N children.
  Literal,  //0 Children.
};

struct ASTNode
{
  using TNodes = std::vector<ASTNode>;
  using TTokens = std::vector<TTokenPair>;

  ASTNodeType mType;
  TNodes mChildren;
  TTokens mTokens;

  static void BuildTree(TTokenGroup::iterator begin, TTokenGroup::iterator end, ASTNode& parent, TypeRegistry& registry);
};

#endif //~__ASTREE_H__

