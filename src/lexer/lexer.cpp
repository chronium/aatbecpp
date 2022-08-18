//
// Created by chronium on 16.08.2022.
//

#include <lexer/lexer.hpp>

#include <string>
#include <algorithm>

using namespace aatbe::source;

namespace aatbe::lexer {

// https://inversepalindrome.com/blog/how-to-format-a-string-in-cpp
template<typename T>
auto convert(T &&t) {
  if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value) {
    return std::forward<T>(t).c_str();
  } else {
    return std::forward<T>(t);
  }
}

// https://inversepalindrome.com/blog/how-to-format-a-string-in-cpp
template<typename... Args>
std::string format_string_internal(const std::string &format, Args &&... args) {
  const auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...) + 1;
  const auto buffer = std::make_unique<char[]>(size);

  std::snprintf(buffer.get(), size, format.c_str(), std::forward<Args>(args)...);

  return std::string(buffer.get(), buffer.get() + size - 1);
}

// https://inversepalindrome.com/blog/how-to-format-a-string-in-cpp
template<typename... Args>
std::string format_string(const std::string &format, Args &&... args) {
  return format_string_internal(format, convert(std::forward<Args>(args))...);
}

Lexer::Lexer(std::shared_ptr<SrcFile> file)
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

Token *Lexer::makeToken(aatbe::lexer::TokenKind kind, const char *valueS, uint64_t valueI) {
  return Lexer::makeToken(kind, new std::string(valueS), valueI);
}

void Lexer::consume_whitespace() {
  while (std::iswspace(this->peek()))
    this->read();
}

Token *Lexer::Next() {
  this->consume_whitespace();

  if (this->peek() == 0)
    return Lexer::makeToken(TokenKind::EndOfFile, "<eof>", EOF);

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

  auto read_escape = [=]() -> char {
    if (this->peek() == '\\') {
      this->read();

      switch (this->read()) {
      case '\\':return '\\';
      case 'n':return '\n';
      case 't':return '\t';
      case 'r':return '\r';
      case '"':return '"';
      case '\'':fprintf(stderr, "Unexpected char delimiter at %zu", this->index);
        exit(1);
      }
    }

    return this->read();
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

    auto rawValueS = new std::string(is_hex ? format_string("0%c%s", next, valueS) : valueS);
    valueS.erase(remove(valueS.begin(), valueS.end(), '_'), valueS.end());

    auto valueI = std::stoll(valueS, nullptr, is_hex ? 16 : 10);
    return Lexer::makeToken(TokenKind::Number,
                            rawValueS,
                            valueI);
  } else if (read_kw("true"))
    return Lexer::makeToken(TokenKind::Boolean, new std::string("true"), true);
  else if (read_kw("false"))
    return Lexer::makeToken(TokenKind::Boolean, new std::string("false"), false);
  else if (cur == '\'') {
    this->read();
    auto c = read_escape();
    valueS += c;

    if (this->peek() != '\'') {
      fprintf(stderr, "Unclosed char delimiter at %zu", this->index);
      exit(1);
    }

    this->read();

    return Lexer::makeToken(TokenKind::Char, new std::string(valueS), c);
  }

  return Lexer::makeToken(TokenKind::Unexpected, new std::string(std::to_string(this->read())));
}

}
