//
// Created by chronium on 18.08.2022.
//

#pragma once

#include <lexer/token.hpp>

#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <vector>

using namespace aatbe::lexer;

namespace aatbe::parser {

enum TerminalKind : int { Boolean, Integer, Char, String, Identifier };

struct Terminal {
  Terminal() = delete;

  Terminal(TerminalKind kind) : kind(kind) {}

  TerminalKind Kind() const { return this->kind; }

private:
  TerminalKind kind;
};

struct BooleanTerm : Terminal {
  explicit BooleanTerm(bool value)
      : Terminal(TerminalKind::Boolean), value(value) {}

  bool Value() const { return value; }

private:
  bool value;
};

struct IntegerTerm : Terminal {
  explicit IntegerTerm(uint64_t value)
      : Terminal(TerminalKind::Integer), value(value) {}

  uint64_t Value() const { return value; }
  uint64_t value;
};

struct CharTerm : Terminal {
  explicit CharTerm(char value)
      : Terminal(TerminalKind::Char), value(value) {}

  char Value() const { return value; }
  char value;
};

struct StringTerm : Terminal {
  explicit StringTerm(std::string value)
      : Terminal(TerminalKind::String), value(std::move(value)) {}

  std::string Value() const { return value; }
  std::string value;
};

struct IdentifierTerm : Terminal {
  explicit IdentifierTerm(std::string value)
      : Terminal(TerminalKind::Identifier), value(std::move(value)) {}

  std::string Value() const { return value; }
  std::string value;
};

struct TerminalNode {
  TerminalNode() = delete;

  template <typename T>
  explicit TerminalNode(T value)
      : value(std::make_shared<T>(std::move(value))) {}

  TerminalKind Kind() const { return value->Kind(); }
  std::shared_ptr<Terminal> Value() const { return value; }

  auto AsBoolean() const -> std::shared_ptr<BooleanTerm> {
    return std::static_pointer_cast<BooleanTerm>(value);
  }

  auto AsInteger() const -> std::shared_ptr<IntegerTerm> {
    return std::static_pointer_cast<IntegerTerm>(value);
  }

  auto AsChar() const -> std::shared_ptr<CharTerm> {
    return std::static_pointer_cast<CharTerm>(value);
  }

  auto AsString() const -> std::shared_ptr<StringTerm> {
    return std::static_pointer_cast<StringTerm>(value);
  }

private:
  std::shared_ptr<Terminal> value;
};


} // namespace aatbe::parser