//
// Created by chronium on 16.08.2022.
//

#include <gtest/gtest.h>

#include <source/source_file.hpp>
#include <lexer/lexer.hpp>

using namespace aatbe::source;
using namespace aatbe::lexer;

std::shared_ptr<SrcFile> makeFile(const char *content) {
  return SrcFile::FromString(content);
}

Lexer *makeLexer(std::shared_ptr<SrcFile> file) {
  return new Lexer(std::move(file));
}

#define EXPECT_TOKEN_SI(tok, kind, valueS, valueI) \
  EXPECT_EQ((tok)->Kind(), (kind));                \
  EXPECT_STREQ((tok)->c_str(), (valueS));          \
  EXPECT_EQ((tok)->ValueI(), (valueI));

TEST(Lexer, EndOfFile) {
  auto lexer = makeLexer(makeFile(""));

  auto eof_tok = lexer->Next();

  EXPECT_TOKEN_SI(eof_tok, TokenKind::EndOfFile, "<eof>", EOF);
}

TEST(Lexer, Number) {
  auto lexer = makeLexer(makeFile("1234_567"));

  auto number_tok = lexer->Next();

  EXPECT_TOKEN_SI(number_tok, TokenKind::Number, "1234_567", 1234567);
}

TEST(Lexer, NegativeNumber) {
  auto lexer = makeLexer(makeFile("-1234_567"));

  auto number_tok = lexer->Next();

  EXPECT_TOKEN_SI(number_tok, TokenKind::Number, "-1234_567", -1234567);
}

TEST(Lexer, HexNumber) {
  auto lexer = makeLexer(makeFile("0x1234"));

  auto number_tok = lexer->Next();

  EXPECT_TOKEN_SI(number_tok, TokenKind::Number, "0x1234", 0x1234);
}

TEST(Lexer, Boolean) {
  auto lexer = makeLexer(makeFile("true false"));

  auto true_tok = lexer->Next();
  auto false_tok = lexer->Next();

  EXPECT_TOKEN_SI(true_tok, TokenKind::Boolean, "true", true);
  EXPECT_TOKEN_SI(false_tok, TokenKind::Boolean, "false", false);
}

TEST(Lexer, Char) {
  auto lexer = makeLexer(makeFile(R"('a' '\n' '0' '\\')"));

  auto a_tok = lexer->Next();
  auto nl_tok = lexer->Next();
  auto zero_tok = lexer->Next();
  auto backslash_tok = lexer->Next();

  EXPECT_TOKEN_SI(a_tok, TokenKind::Char, "a", 'a');
  EXPECT_TOKEN_SI(nl_tok, TokenKind::Char, "\n", '\n');
  EXPECT_TOKEN_SI(zero_tok, TokenKind::Char, "0", '0');
  EXPECT_TOKEN_SI(backslash_tok, TokenKind::Char, "\\", '\\');
}
