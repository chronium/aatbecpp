//
// Created by chronium on 26.08.2022.
//

#include <gtest/gtest.h>

#include "base.hpp"

#include <parser/parser.hpp>

using namespace aatbe::parser;

TEST(FunctionParser, ParameterParser) {
  auto tokens = makeTokens("a: int32 b: int32");
  Parser parser(tokens);

  auto a = ParseParameter(parser);
  auto b = ParseParameter(parser);

  EXPECT_TRUE(a);
  EXPECT_EQ(std::get<0>(a.Value()), "a");
  EXPECT_EQ(std::get<1>(a.Value())->Kind(), TypeKind::Int32);

  EXPECT_TRUE(b);
  EXPECT_EQ(std::get<0>(b.Value()), "b");
  EXPECT_EQ(std::get<1>(b.Value())->Kind(), TypeKind::Int32);
}

TEST(FunctionParser, Simple) {
  auto tokens = makeTokens("fn foo");
  Parser parser(tokens);

  auto function = ParseModuleStatement(parser);

  EXPECT_TRUE(function);
  EXPECT_FALSE(Dig(function, Function, IsExtern));
  EXPECT_EQ(Dig(function, Function, Name), "foo");
  EXPECT_EQ(Dig(function, Function, Parameters)->Size(), 0);
  EXPECT_EQ(Dig(function, Function, ReturnType)->Kind(), TypeKind::Unit);
  EXPECT_FALSE(Dig(function, Function, Body).has_value());
}

TEST(FunctionParser, Extern) {
  auto tokens = makeTokens("extern fn foo");
  Parser parser(tokens);

  auto function = ParseModuleStatement(parser);

  EXPECT_TRUE(function);
  EXPECT_TRUE(Dig(function, Function, IsExtern));
  EXPECT_EQ(Dig(function, Function, Name), "foo");
  EXPECT_EQ(Dig(function, Function, Parameters)->Size(), 0);
  EXPECT_EQ(Dig(function, Function, ReturnType)->Kind(), TypeKind::Unit);
  EXPECT_FALSE(Dig(function, Function, Body).has_value());
}

TEST(FunctionParser, Body) {
  auto tokens = makeTokens("fn foo = ()");
  Parser parser(tokens);

  auto function = ParseModuleStatement(parser);

  EXPECT_TRUE(function);
  EXPECT_FALSE(Dig(function, Function, IsExtern));
  EXPECT_EQ(Dig(function, Function, Name), "foo");
  EXPECT_EQ(Dig(function, Function, Parameters)->Size(), 0);
  EXPECT_EQ(Dig(function, Function, ReturnType)->Kind(), TypeKind::Unit);
  EXPECT_TRUE(Dig(function, Function, Body).has_value());
}

TEST(FunctionParser, ReturnType) {
  auto tokens = makeTokens("fn foo -> str = ()");
  Parser parser(tokens);

  auto function = ParseModuleStatement(parser);

  EXPECT_TRUE(function);
  EXPECT_FALSE(Dig(function, Function, IsExtern));
  EXPECT_EQ(Dig(function, Function, Name), "foo");
  EXPECT_EQ(Dig(function, Function, Parameters)->Size(), 0);
  EXPECT_EQ(Dig(function, Function, ReturnType)->Kind(), TypeKind::Str);
  EXPECT_TRUE(Dig(function, Function, Body).has_value());
}

TEST(FunctionParser, Arguments) {
  auto tokens = makeTokens("fn foo (fmt: str)");
  Parser parser(tokens);

  auto function = ParseModuleStatement(parser);

  EXPECT_TRUE(function);
  EXPECT_FALSE(Dig(function, Function, IsExtern));
  EXPECT_EQ(Dig(function, Function, Name), "foo");
  EXPECT_EQ(Dig(function, Function, Parameters)->Size(), 1);
  EXPECT_EQ(Dig(function, Function, ReturnType)->Kind(), TypeKind::Unit);
  EXPECT_FALSE(Dig(function, Function, Body).has_value());
}

TEST(FunctionParser, VariadicArgument) {
  auto tokens = makeTokens("fn foo (fmt: str, ...)");
  Parser parser(tokens);

  auto function = ParseModuleStatement(parser);

  EXPECT_TRUE(function);
  EXPECT_FALSE(Dig(function, Function, IsExtern));
  EXPECT_TRUE(Dig(function, Function, IsVariadic));
  EXPECT_EQ(Dig(function, Function, Name), "foo");
  EXPECT_EQ(Dig(function, Function, Parameters)->Size(), 1);
  EXPECT_EQ(Dig(function, Function, ReturnType)->Kind(), TypeKind::Unit);
  EXPECT_FALSE(Dig(function, Function, Body).has_value());
}
