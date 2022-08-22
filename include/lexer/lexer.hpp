//
// Created by chronium on 16.08.2022.
//

#pragma once

#include <cstdint>
#include <string>

#include <lexer/token.hpp>
#include <source/source_file.hpp>

namespace aatbe::lexer {

using namespace aatbe::source;

class Lexer {
public:
  Lexer(std::shared_ptr<SrcFile> file);

  std::vector<Token> Lex();
  Token *Next();

private:
  char peek(off_t off = 0);
  char read();

  void consume_whitespace();

  Token *makeToken(TokenKind kind, std::string *valueS);
  Token *makeToken(TokenKind kind, const char *valueS);
  Token *makeToken(TokenKind kind, std::string *valueS, uint64_t valueI);
  Token *makeToken(aatbe::lexer::TokenKind kind, const char *valueS,
                   uint64_t valueI);

  std::shared_ptr<SrcFile> file;

  size_t index;
  size_t last_index;
};

} // namespace aatbe::lexer
