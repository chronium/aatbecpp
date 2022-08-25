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

enum TerminalKind : int { Boolean, Integer, Character, String, Identifier };

struct Terminal {
  Terminal() = default;
  Terminal(Terminal &&) = delete;
  Terminal(Terminal const &) = delete;
  Terminal &operator=(Terminal const &) = delete;

  virtual ~Terminal() = default;

  virtual TerminalKind Kind() const = 0;
};

struct BooleanTerm : public Terminal {
  explicit BooleanTerm(bool value)
      : kind(TerminalKind::Boolean), value(value) {}

  bool Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

private:
  TerminalKind kind;
  bool value;
};

struct IntegerTerm : public Terminal {
  explicit IntegerTerm(uint64_t value)
      : kind(TerminalKind::Integer), value(value) {}

  uint64_t Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

private:
  TerminalKind kind;
  uint64_t value;
};

struct CharTerm : public Terminal {
  explicit CharTerm(char value) : kind(TerminalKind::Character), value(value) {}

  char Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

private:
  TerminalKind kind;
  char value;
};

struct StringTerm : public Terminal {
  explicit StringTerm(std::string value)
      : kind(TerminalKind::String), value(std::move(value)) {}

  std::string Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

private:
  TerminalKind kind;
  std::string value;
};

struct IdentifierTerm : public Terminal {
  explicit IdentifierTerm(std::string value)
      : kind(TerminalKind::Identifier), value(std::move(value)) {}

  std::string Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

private:
  TerminalKind kind;
  std::string value;
};

struct TerminalNode {
  TerminalNode() = delete;

  template <typename T> explicit TerminalNode(T value) : value(value) {}

  TerminalKind Kind() const { return value->Kind(); }
  auto Value() const { return value; }

  auto AsBoolean() const -> BooleanTerm * { return (BooleanTerm *)value; }

  auto AsInteger() const -> IntegerTerm * { return (IntegerTerm *)value; }

  auto AsChar() const -> CharTerm * { return (CharTerm *)value; }

  auto AsString() const -> StringTerm * { return (StringTerm *)value; }

  auto AsIdentifier() const -> IdentifierTerm * {
    return (IdentifierTerm *)value;
  }

private:
  Terminal *value;
};

ParseResult<TerminalNode *> ParseTerminal(Parser &parser);

} // namespace aatbe::parser