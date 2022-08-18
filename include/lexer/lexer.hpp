//
// Created by chronium on 16.08.2022.
//

#pragma once

#include <string>
#include <cstdint>

#include <source/source_file.hpp>
#include <lexer/token.hpp>

namespace aatbe::lexer {

using namespace aatbe::source;

class Lexer {
public:
  Lexer(std::shared_ptr<SrcFile> file);

  Token *Next();

private:
  char peek(off_t off = 0);
  char read();

  void consume_whitespace();

  Token *makeToken(TokenKind kind, std::string *valueS);
  Token *makeToken(TokenKind kind, const char *valueS);
  Token *makeToken(TokenKind kind, std::string *valueS, uint64_t valueI);
  Token *makeToken(aatbe::lexer::TokenKind kind, const char *valueS, uint64_t valueI);

  std::shared_ptr<SrcFile> file;

  size_t index;
  size_t last_index;
};

}
