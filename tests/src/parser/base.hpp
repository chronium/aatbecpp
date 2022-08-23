//
// Created by chronium on 23.08.2022.
//

#pragma once

#include <lexer/lexer.hpp>

inline std::vector<Token *> makeTokens(const char *content) {
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
