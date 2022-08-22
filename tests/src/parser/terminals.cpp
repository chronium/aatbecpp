//
// Created by chronium on 18.08.2022.
//

#include <gtest/gtest.h>

#include <lexer/lexer.hpp>
#include <parser/parser.hpp>

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

TEST(TerminalParser, Boolean) {
  auto tokens = makeTokens("true false");
  Parser parser(tokens);

  auto true_atom = parser.ParseTerminal();
  auto false_atom = parser.ParseTerminal();

  EXPECT_TRUE(true_atom);
  EXPECT_EQ(true_atom.Kind(), TerminalKind::Boolean);
  EXPECT_EQ(Unwrap(true_atom, Boolean), true);

  EXPECT_TRUE(false_atom);
  EXPECT_EQ(false_atom.Kind(), TerminalKind::Boolean);
  EXPECT_EQ(Unwrap(false_atom, Boolean), false);
}

TEST(TerminalParser, Integer) {
  auto tokens = makeTokens("1234 -267 0x1234");
  Parser parser(tokens);

  auto one = parser.ParseTerminal();
  auto two = parser.ParseTerminal();
  auto three = parser.ParseTerminal();

  EXPECT_TRUE(one);
  EXPECT_EQ(one.Kind(), TerminalKind::Integer);
  EXPECT_EQ(Unwrap(one, Integer), 1234);

  EXPECT_TRUE(two);
  EXPECT_EQ(two.Kind(), TerminalKind::Integer);
  EXPECT_EQ(Unwrap(two, Integer), -267);

  EXPECT_TRUE(three);
  EXPECT_EQ(three.Kind(), TerminalKind::Integer);
  EXPECT_EQ(Unwrap(three, Integer), 0x1234);
}

TEST(TerminalParser, Char) {
  auto tokens = makeTokens("'a' '\\n' '\\t'");
  Parser parser(tokens);

  auto a = parser.ParseTerminal();
  auto b = parser.ParseTerminal();
  auto c = parser.ParseTerminal();

  EXPECT_TRUE(a);
  EXPECT_EQ(a.Kind(), TerminalKind::Char);
  EXPECT_EQ(Unwrap(a, Char), 'a');

  EXPECT_TRUE(b);
  EXPECT_EQ(b.Kind(), TerminalKind::Char);
  EXPECT_EQ(Unwrap(b, Char), '\n');

  EXPECT_TRUE(c);
  EXPECT_EQ(c.Kind(), TerminalKind::Char);
  EXPECT_EQ(Unwrap(c, Char), '\t');
}

TEST(TerminalParser, String) {
  auto tokens = makeTokens("\"hello\" \"world\" \"\\\\\\n\"");
  Parser parser(tokens);

  auto a = parser.ParseTerminal();
  auto b = parser.ParseTerminal();
  auto c = parser.ParseTerminal();

  EXPECT_TRUE(a);
  EXPECT_EQ(a.Kind(), TerminalKind::String);
  EXPECT_STREQ(Unwrap(a, String).c_str(), "hello");

  EXPECT_TRUE(b);
  EXPECT_EQ(b.Kind(), TerminalKind::String);
  EXPECT_STREQ(Unwrap(b, String).c_str(), "world");

  EXPECT_TRUE(c);
  EXPECT_EQ(c.Kind(), TerminalKind::String);
  EXPECT_STREQ(Unwrap(c, String).c_str(), "\\\n");
}