//
// Created by chronium on 16.08.2022.
//

#include <gtest/gtest.h>

#include <source/source_file.hpp>
#include <lexer/lexer.hpp>

using namespace aatbe::source;
using namespace aatbe::lexer;

std::unique_ptr<SrcFile> makeFile(const char *content) {
  return SrcFile::FromString(content);
}

Lexer *makeLexer(std::unique_ptr<SrcFile> file) {
  return new Lexer(std::move(file));
}

#define EXPECT_STREQ_IN(LEFT, RIGHT) EXPECT_STREQ((LEFT)->get()->c_str(), RIGHT);

TEST(Lexer, EndOfFile) {
  auto lexer = makeLexer(makeFile(""));

  auto eof_tok = lexer->Next();

  EXPECT_EQ(eof_tok->Kind(), TokenKind::EndOfFile);
}

TEST(Lexer, Number) {
  auto lexer = makeLexer(makeFile("1234_567"));

  auto number_tok = lexer->Next();

  EXPECT_EQ(number_tok->Kind(), TokenKind::Number);
  EXPECT_EQ(number_tok->ValueI(), 1234567);
}

TEST(Lexer, NegativeNumber) {
  auto lexer = makeLexer(makeFile("-1234_567"));

  auto number_tok = lexer->Next();

  EXPECT_EQ(number_tok->Kind(), TokenKind::Number);
  EXPECT_EQ(number_tok->ValueI(), -1234567);
}

TEST(Lexer, HexNumber) {
  auto lexer = makeLexer(makeFile("0x1234"));

  auto number_tok = lexer->Next();

  EXPECT_EQ(number_tok->Kind(), TokenKind::Number);
  EXPECT_EQ(number_tok->ValueI(), 0x1234);
}

TEST(Lexer, Boolean) {
  auto lexer = makeLexer(makeFile("true false"));

  auto true_tok = lexer->Next();
  auto false_tok = lexer->Next();

  EXPECT_EQ(true_tok->Kind(), TokenKind::Boolean);
  EXPECT_STREQ_IN(true_tok->ValueS(), "true");
  EXPECT_EQ(true_tok->ValueI(), 1);

  EXPECT_EQ(false_tok->Kind(), TokenKind::Boolean);
  EXPECT_STREQ_IN(false_tok->ValueS(), "false");
  EXPECT_EQ(false_tok->ValueI(), 0);
}
