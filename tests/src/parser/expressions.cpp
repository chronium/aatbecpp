//
// Created by chronium on 25.08.2022.
//

#include <gtest/gtest.h>

#include "base.hpp"
#include <parser/expression.hpp>
#include <parser/parser.hpp>

using namespace aatbe::parser;

TEST(ExpressionParser, ParseTuple) {
  auto tokens = makeTokens("(1, 2, 3)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Tuple);
  EXPECT_EQ(Unwrap(expr, Tuple).size(), 3);

  auto children = Unwrap(expr, Tuple);

  EXPECT_EQ(children[0]->AsAtom()->Value()->AsInteger()->Value(), 1);
  EXPECT_EQ(children[1]->AsAtom()->Value()->AsInteger()->Value(), 2);
  EXPECT_EQ(children[2]->AsAtom()->Value()->AsInteger()->Value(), 3);
}

TEST(ExpressionParser, Call) {
  auto tokens = makeTokens(R"(printf("%d", 1))");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Call);

  auto calleeNode = Dig(expr, Call, Callee)->AsAtom();
  auto argsNode = Dig(expr, Call, Args)->AsTuple();

  EXPECT_EQ(Indirect(calleeNode, Identifier), "printf");
  EXPECT_EQ(argsNode->Size(), 2);
}

TEST(ExpressionParser, Block) {
  auto tokens = makeTokens(R"({ 1; 2; 3 })");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Block);
  EXPECT_EQ(Dig(expr, Block, Size), 3);
}
