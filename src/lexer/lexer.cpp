//
// Created by chronium on 16.08.2022.
//

#include <lexer/lexer.hpp>

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

using namespace aatbe::source;

namespace aatbe::lexer {

// https://inversepalindrome.com/blog/how-to-format-a-string-in-cpp
template <typename T> auto convert(T &&t) {
  if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>,
                             std::string>::value) {
    return std::forward<T>(t).c_str();
  } else {
    return std::forward<T>(t);
  }
}

// https://inversepalindrome.com/blog/how-to-format-a-string-in-cpp
template <typename... Args>
std::string format_string_internal(const std::string &format, Args &&...args) {
  const auto size =
      std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...) +
      1;
  const auto buffer = std::make_unique<char[]>(size);

  std::snprintf(buffer.get(), size, format.c_str(),
                std::forward<Args>(args)...);

  return std::string(buffer.get(), buffer.get() + size - 1);
}

// https://inversepalindrome.com/blog/how-to-format-a-string-in-cpp
template <typename... Args>
std::string format_string(const std::string &format, Args &&...args) {
  return format_string_internal(format, convert(std::forward<Args>(args))...);
}

std::vector<std::string> symbols = {
    "++", "--", "+",  "-",  "*",  "/",  "=",  "==", "!=",  ">",  "<", "<=",
    ">=", "&&", "||", "!",  "->", "<-", "|>", "<|", "...", "..", ".", "&",
    "|",  "^",  "~",  ">>", "<<", "{",  "}",  "[",  "]",   "(",  ")", ",",
    ";",  "%",  "+=", "-=", "*=", "/=", "%=", "@",  ":"};

std::vector<std::string> keywords = {
    "as",       "fn",    "if",     "in",     "for",    "use",    "val",
    "var",      "else",  "enum",   "then",   "from",   "type",   "break",
    "const",    "while", "global", "return", "struct", "export", "module",
    "continue", "ref",   "ptr",    "extern", "loop"};

Lexer::Lexer(std::shared_ptr<SrcFile> file)
    : file(std::move(file)), index(0), last_index(0) {
  std::sort(symbols.begin(), symbols.end());
  std::reverse(symbols.begin(), symbols.end());
  std::sort(keywords.begin(), keywords.end());
  std::reverse(keywords.begin(), keywords.end());
}

char Lexer::peek(off_t off) { return file->Char(this->index + off); }

char Lexer::read() { return file->Char(this->index++); }

Token *Lexer::makeToken(TokenKind kind, std::string *valueS) {
  return Lexer::makeToken(kind, valueS, 0);
}

Token *Lexer::makeToken(TokenKind kind, const char *valueS) {
  return Lexer::makeToken(kind, new std::string(valueS), 0);
}

Token *Lexer::makeToken(TokenKind kind, std::string *valueS, uint64_t valueI) {
  auto tok = new Token(kind, valueS, valueI, this->file, this->last_index,
                       this->index);
  this->last_index = this->index;

  return tok;
}

Token *Lexer::makeToken(TokenKind kind, const char *valueS, uint64_t valueI) {
  return Lexer::makeToken(kind, new std::string(valueS), valueI);
}

void Lexer::consume_whitespace() {
  while (std::isspace(this->peek()))
    this->read();
  this->last_index = this->index;
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
      case '\\':
        return '\\';
      case 'n':
        return '\n';
      case 't':
        return '\t';
      case 'r':
        return '\r';
      case '"':
        return '"';
      case '\'':
        fprintf(stderr, "Unexpected char delimiter at %zu", this->index);
        exit(1);
      }
    }

    return this->read();
  };

  auto ident_start = [=](char c) -> bool {
    return std::isalpha(c) || c == '_';
  };

  auto ident_continue = [=](char c) -> bool {
    return ident_start(c) || std::isdigit(c);
  };

  auto ident_end = [=](char c) -> bool {
    return ident_continue(c) || c == '?' || c == '!';
  };

  if (std::isdigit(cur) || (cur == '-' && std::isdigit(next)) ||
      (cur == '0' && is_hex_start(next))) {

    auto is_hex = is_hex_start(next);

    if (!is_hex)
      valueS += this->read();
    else {
      this->read(); // read 0
      this->read(); // read x/X
    }

    while (isxdigit(this->peek()) || this->peek() == '_')
      valueS += this->read();

    auto rawValueS =
        new std::string(is_hex ? format_string("0%c%s", next, valueS) : valueS);
    valueS.erase(remove(valueS.begin(), valueS.end(), '_'), valueS.end());

    auto valueI = std::stoll(valueS, nullptr, is_hex ? 16 : 10);
    return Lexer::makeToken(TokenKind::Number, rawValueS, valueI);
  } else if (read_kw("true"))
    return Lexer::makeToken(TokenKind::Boolean, new std::string("true"), true);
  else if (read_kw("false"))
    return Lexer::makeToken(TokenKind::Boolean, new std::string("false"),
                            false);
  else if (cur == '\'') {
    this->read();
    auto c = read_escape();
    valueS += c;

    if (this->read() != '\'') {
      fprintf(stderr, "Unclosed char delimiter at %zu", this->index);
      exit(1);
    }

    this->read();

    return Lexer::makeToken(TokenKind::Char, new std::string(valueS), c);
  } else if (cur == '"') {
    this->read();

    while (this->peek() != '"' && this->peek())
      valueS += read_escape();

    if (this->read() != '\"') {
      fprintf(stderr, "Unclosed string delimiter at %zu", this->index);
      exit(1);
    }

    return Lexer::makeToken(TokenKind::String, new std::string(valueS));
  }

  for (const auto &symbol : symbols) {
    if (read_kw(symbol.c_str())) {
      return Lexer::makeToken(TokenKind::Symbol, new std::string(symbol));
    }
  }

  if (ident_start(cur)) {
    while (ident_continue(this->peek()))
      valueS += this->read();

    if (ident_end(this->peek()))
      valueS += this->read();

    if (std::find(keywords.begin(), keywords.end(), valueS) != keywords.end())
      return Lexer::makeToken(TokenKind::Keyword, valueS.c_str());
    else
      return Lexer::makeToken(TokenKind::Identifier, valueS.c_str());
  }

  return Lexer::makeToken(TokenKind::Unexpected,
                          new std::string(std::to_string(this->read())));
}

std::vector<Token *> Lexer::Lex() {
  std::vector<Token *> tokens;

  while (true) {
    auto tok = this->Next();

    if (tok->Kind() == TokenKind::EndOfFile)
      break;

    tokens.push_back(tok);
  }

  return tokens;
}

} // namespace aatbe::lexer
