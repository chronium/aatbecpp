//
// Created by chronium on 05.09.2022.
//

#include <gtest/gtest.h>

#include "base.hpp"

#include <parser/parser.hpp>

using namespace aatbe::parser;

TEST(ModuleParser, ParseStruct) {
  auto tokens = makeTokens(R"(struct Test { a: foo; public b: baz })");
  Parser parser(tokens);

  auto test = ParseModuleStatement(parser);

  ASSERT_TRUE(test);
  EXPECT_EQ(Dig(test, Struct, Name), "Test");
  EXPECT_EQ(Dig(test, Struct, Members)->Size(), 2);
}
