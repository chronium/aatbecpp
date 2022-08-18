//
// Created by chronium on 18.08.2022.
//

#include <gtest/gtest.h>

#include <lexer/lexer.hpp>
#include <parser/atoms.hpp>

#include <vector>

using namespace aatbe::lexer;
using namespace aatbe::source;
using namespace aatbe::parser;

std::vector<Token *> makeTokens(const char *content) {
  auto file = SrcFile::FromString(content);
  auto lexer = new Lexer(std::move(file));
  auto tokens = std::vector<Token *>();
  while (true) {
    auto tok = lexer->Next();
    if (tok->Kind() == TokenKind::EndOfFile) {
      break;
    }
    tokens.push_back(tok);
  }
  return tokens;
}

TEST(AtomsParser, Boolean) {
  auto tokens = makeTokens("true false");

  auto true_atom = ParseBoolean(tokens);
  auto false_atom = ParseBoolean(true_atom.Rest());

  EXPECT_TRUE(true_atom);
  EXPECT_EQ(true_atom.Kind(), AtomKind::Boolean);
  EXPECT_EQ(true_atom.Value(), true);

  EXPECT_TRUE(false_atom);
  EXPECT_EQ(false_atom.Kind(), AtomKind::Boolean);
  EXPECT_EQ(false_atom.Value(), false);
}
