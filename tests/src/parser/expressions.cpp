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

TEST(ExpressionParser, CallNoArgs) {
  auto tokens = makeTokens(R"(printf())");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Call);

  auto calleeNode = Dig(expr, Call, Callee)->AsAtom();
  auto argsNode = Dig(expr, Call, Args)->AsTuple();

  EXPECT_EQ(Indirect(calleeNode, Identifier), "printf");
  EXPECT_EQ(argsNode->Size(), 0);
}

TEST(ExpressionParser, Block) {
  auto tokens = makeTokens(R"({ 1 2; 3 })");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Block);
  EXPECT_EQ(Dig(expr, Block, Size), 3);
}

TEST(ExpressionParser, IfSimple) {
  auto tokens = makeTokens(R"(if true 1)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::If);
  EXPECT_EQ(Dig(expr, If, Size), 1);
}

TEST(ExpressionParser, IfThenSimple) {
  auto tokens = makeTokens(R"(if true then 1)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::If);
  EXPECT_EQ(Dig(expr, If, Size), 1);
}

TEST(ExpressionParser, IfElse) {
  auto tokens = makeTokens(R"(if true 1 else 0)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::If);
  EXPECT_EQ(Dig(expr, If, Size), 2);
}

TEST(ExpressionParser, IfThenElse) {
  auto tokens = makeTokens(R"(if true then 1 else 0)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::If);
  EXPECT_EQ(Dig(expr, If, Size), 2);
}

TEST(ExpressionParser, IfElseIfElse) {
  auto tokens = makeTokens(R"(if true 1 else if false 0 else 1)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::If);
  EXPECT_EQ(Dig(expr, If, Size), 3);
}

TEST(ExpressionParser, Loop) {
  auto tokens = makeTokens(R"(loop 1)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Loop);
}

TEST(ExpressionParser, Accessor) {
  auto tokens = makeTokens(R"(foo.bar)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Accessor);
  EXPECT_EQ(Dig(expr, Accessor, Object)->Format(), "foo");
  EXPECT_EQ(Dig(expr, Accessor, Accessor), "bar");
}

TEST(ExpressionParser, AccessorChain) {
  auto tokens = makeTokens(R"(foo.bar.baz)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Accessor);
  EXPECT_EQ(Dig(expr, Accessor, Object)->Format(), "foo.bar");
  EXPECT_EQ(Dig(expr, Accessor, Accessor), "baz");
}

TEST(ExpressionParser, AccessorChainWithExpr) {
  auto tokens = makeTokens(R"(foo.bar().baz)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Accessor);
  EXPECT_EQ(Dig(expr, Accessor, Object)->Kind(), ExpressionKind::Call);
  EXPECT_EQ(Dig(expr, Accessor, Accessor), "baz");
}

TEST(ExpressionParser, AccessorOnFuncall) {
  auto tokens = makeTokens(R"(foo().bar)");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Accessor);
  EXPECT_EQ(Dig(expr, Accessor, Object)->Kind(), ExpressionKind::Call);
  EXPECT_EQ(Dig(expr, Accessor, Accessor), "bar");
}

TEST(ExpressionParser, CallMember) {
  auto tokens = makeTokens(R"(foo.bar.baz())");
  Parser parser(tokens);

  auto expr = ParseExpression(parser);
  auto callee = Dig(expr, Call, Callee);

  EXPECT_TRUE(expr);
  EXPECT_EQ(expr.Kind(), ExpressionKind::Call);
  EXPECT_EQ(callee->Kind(), ExpressionKind::Accessor);
  EXPECT_EQ(callee->AsAccessor()->Accessor(), "bar");
  EXPECT_EQ(Dig(expr, Call, Args)->AsTuple()->Size(), 0);
}
