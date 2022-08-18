//
// Created by chronium on 16.08.2022.
//

#pragma once

#include <memory>
#include <source/source_file.hpp>
#include <string>
#include <tuple>

namespace aatbe::lexer {

using namespace aatbe::source;

enum TokenKind {
  EndOfFile,
  Number,
  Unexpected,
  Boolean,
  Char,
  String,
  Identifier,
  Keyword,
  Symbol,
};

class Token {
public:
  Token(TokenKind kind, std::string *valueS, uint64_t valueI,
        std::shared_ptr<SrcFile> &file, size_t start, size_t end);

  Token(TokenKind kind, std::string *valueS, uint64_t valueI, size_t start,
        size_t end);

  bool operator==(Token const &);

  bool operator!=(Token const &rhs) { return !(this == &rhs); }

  TokenKind Kind() const { return this->kind; }

  const std::string &ValueS() const { return *this->valueS.get(); }

  const char *c_str() const { return this->valueS.get()->c_str(); }

  uint64_t ValueI() const { return this->valueI; }

  size_t Start() const { return this->start; }
  size_t End() const { return this->end; }

private:
  const TokenKind kind;
  std::unique_ptr<std::string> valueS;
  uint64_t valueI;

  std::weak_ptr<SrcFile> file;

  size_t start;
  size_t end;
};

} // namespace aatbe::lexer
