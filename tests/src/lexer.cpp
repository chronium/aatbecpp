//
// Created by chronium on 16.08.2022.
//

#include <gtest/gtest.h>

#include <lexer/lexer.hpp>

#include <vector>

using namespace aatbe::source;
using namespace aatbe::lexer;

std::shared_ptr<SrcFile> makeFile(const char *content) {
  return SrcFile::FromString(content);
}

Lexer *makeLexer(std::shared_ptr<SrcFile> file) {
  return new Lexer(std::move(file));
}

#define EXPECT_TOKEN_SI(tok, kind, valueS, valueI)                             \
  EXPECT_EQ((tok)->Kind(), (kind));                                            \
  EXPECT_STREQ((tok)->c_str(), (valueS));                                      \
  EXPECT_EQ((tok)->ValueI(), (valueI));

#define EXPECT_TOKEN_S(tok, kind, valueS)                                      \
  EXPECT_EQ((tok)->Kind(), (kind));                                            \
  EXPECT_STREQ((tok)->c_str(), (valueS));

#define EXPECT_TOKEN_SYM(tok, valueS)                                          \
  EXPECT_EQ((tok)->Kind(), TokenKind::Symbol);                                 \
  EXPECT_STREQ((tok)->c_str(), (valueS));

#define EXPECT_TOKEN_KW(tok, valueS)                                           \
  EXPECT_STREQ((tok)->c_str(), (valueS));                                      \
  EXPECT_EQ((tok)->Kind(), TokenKind::Keyword);

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

TEST(Lexer, String) {
  auto lexer = makeLexer(makeFile(R"("a" "abc" "a\nb")"));

  auto a_tok = lexer->Next();
  auto abc_tok = lexer->Next();
  auto a_esc_b_tok = lexer->Next();

  EXPECT_TOKEN_S(a_tok, TokenKind::String, "a");
  EXPECT_TOKEN_S(abc_tok, TokenKind::String, "abc");
  EXPECT_TOKEN_S(a_esc_b_tok, TokenKind::String, "a\nb");
}

TEST(Lexer, Identifier) {
  auto lexer =
      makeLexer(makeFile("ident _ident ident_ id3nt _1d3nt_ ident? ident!"));

  auto ident_tok = lexer->Next();
  auto _ident_tok = lexer->Next();
  auto ident__tok = lexer->Next();
  auto id3nt_tok = lexer->Next();
  auto _1d3nt_tok = lexer->Next();
  auto ident_q_tok = lexer->Next();
  auto ident_excl_tok = lexer->Next();

  EXPECT_TOKEN_S(ident_tok, TokenKind::Identifier, "ident");
  EXPECT_TOKEN_S(_ident_tok, TokenKind::Identifier, "_ident");
  EXPECT_TOKEN_S(ident__tok, TokenKind::Identifier, "ident_");
  EXPECT_TOKEN_S(id3nt_tok, TokenKind::Identifier, "id3nt");
  EXPECT_TOKEN_S(_1d3nt_tok, TokenKind::Identifier, "_1d3nt_");
  EXPECT_TOKEN_S(ident_q_tok, TokenKind::Identifier, "ident?");
  EXPECT_TOKEN_S(ident_excl_tok, TokenKind::Identifier, "ident!");
}

TEST(Lexer, Symbol) {
  auto input = "+ - * / % = == != < > <= >= && || ! "
               "-> <- "
               "|> <| "
               ".. ... . "
               "& | ^ ~ "
               ">> >> "
               "{ } [ ] ( ) "
               ", ; @ : "
               "+= -= *= /= %= ";
  auto lexer = makeLexer(makeFile(input));

  std::stringstream stream(input);
  std::istream_iterator<std::string> begin(stream);
  std::istream_iterator<std::string> end;

  auto symbols = std::vector<std::string>(begin, end);

  for (const auto& symbol : symbols) {
    auto symbol_tok = lexer->Next();
    EXPECT_TOKEN_SYM(symbol_tok, symbol.c_str());
  }
}

TEST(Lexer, Keyword) {
  auto input = "if else while for break continue return then "
               "fn in var val from use module export "
               "const global type struct enum as "
               "extern ref ptr loop public";
  auto lexer = makeLexer(makeFile(input));

  std::stringstream stream(input);
  std::istream_iterator<std::string> begin(stream);
  std::istream_iterator<std::string> end;

  auto keywords = std::vector<std::string>(begin, end);

  for (const auto &keyword : keywords) {
    auto keyword_tok = lexer->Next();
    EXPECT_TOKEN_KW(keyword_tok, keyword.c_str());
  }
}

TEST(Lexer, Mixed) {
  auto input = "int8";
  auto lexer = makeLexer(makeFile(input));

  auto int8_tok = lexer->Next();

  EXPECT_TOKEN_S(int8_tok, TokenKind::Identifier, "int8");
}
