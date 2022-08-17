//
// Created by chronium on 16.08.2022.
//

#include <lexer/lexer.hpp>

#include <string>
#include <algorithm>

using namespace aatbe::source;

namespace aatbe::lexer {

Lexer::Lexer(std::unique_ptr<SrcFile> file)
    : file(std::move(file)),
      index(0),
      last_index(0) {
}

char Lexer::peek(off_t off) {
  return file->Char(this->index + off);
}

char Lexer::read() {
  return file->Char(this->index++);
}

Token *Lexer::makeToken(TokenKind kind, std::string *valueS) {
  return Lexer::makeToken(kind, valueS, 0);
}

Token *Lexer::makeToken(TokenKind kind, const char *valueS) {
  return Lexer::makeToken(kind, new std::string(valueS), 0);
}

Token *Lexer::makeToken(aatbe::lexer::TokenKind kind, std::string *valueS, uint64_t valueI) {
  auto tok = new Token(kind, valueS, valueI, this->file, this->last_index, this->index);
  this->last_index = this->index;

  return tok;
}

void Lexer::consume_whitespace() {
  while (std::iswspace(this->peek()))
    this->read();
}

Token *Lexer::Next() {
  this->consume_whitespace();

  if (this->peek() == 0)
    return Lexer::makeToken(TokenKind::EndOfFile, "<eof>");

  auto cur = this->peek();
  auto next = this->peek(1);
  std::string valueS;

  auto is_hex_start = [](char c) -> bool { return c == 'x' || c == 'X'; };

  auto matches_kw = [=](const char *kw) -> bool {
    return this->file->Contains(this->index, kw);
  };

  auto read_kw = [=](const char *kw) -> bool {
    if (matches_kw(kw)) {
      this->index += std::strlen(kw);
      return true;
    }

    return false;
  };

  if (std::isdigit(cur) ||
      (cur == '-' && std::isdigit(next)) ||
      (cur == '0' && is_hex_start(next))) {

    auto is_hex = is_hex_start(next);

    if (!is_hex)
      valueS += this->read();
    else {
      this->read(); // read 0
      this->read(); // read x/X
    }

    while (ishexnumber(this->peek()) || this->peek() == '_')
      valueS += this->read();

    valueS.erase(remove(valueS.begin(), valueS.end(), '_'), valueS.end());

    auto valueI = std::stoll(valueS, nullptr, is_hex ? 16 : 10);
    return Lexer::makeToken(TokenKind::Number, new std::string(valueS), valueI);
  } else if (read_kw("true"))
    return Lexer::makeToken(TokenKind::Boolean, new std::string("true"), 1);
  else if (read_kw("false"))
    return Lexer::makeToken(TokenKind::Boolean, new std::string("false"), 0);

  return Lexer::makeToken(TokenKind::Unexpected, new std::string(std::to_string(this->read())));
}

}
