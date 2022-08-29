//
// Created by chronium on 18.08.2022.
//

#pragma once

#include <lexer/token.hpp>
#include <parser/type.hpp>

#include <vector>

using namespace aatbe::lexer;

namespace aatbe::parser {

void replace_all(
    std::string& s,
    std::string const& toReplace,
    std::string const& replaceWith
);

enum TerminalKind : int { Boolean, Integer, Character, String, Identifier, UnitVal };

struct Terminal {
  Terminal() = default;
  Terminal(Terminal &&) = delete;
  Terminal(Terminal const &) = delete;
  Terminal &operator=(Terminal const &) = delete;

  virtual ~Terminal() = default;

  virtual TerminalKind Kind() const = 0;
  virtual std::string Format() const = 0;
};

struct BooleanTerm : public Terminal {
  explicit BooleanTerm(bool value)
      : kind(TerminalKind::Boolean), value(value) {}

  bool Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

  std::string Format() const override { return value ? "true" : "false"; }

private:
  TerminalKind kind;
  bool value;
};

struct IntegerTerm : public Terminal {
  explicit IntegerTerm(uint64_t value, TypeNode *type)
      : kind(TerminalKind::Integer), value(value), type(type) {}

  uint64_t Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }
  TypeNode *Type() const { return type; }

  std::string Format() const override { return std::to_string(value); }

private:
  TerminalKind kind;
  uint64_t value;
  TypeNode *type;
};

struct CharTerm : public Terminal {
  explicit CharTerm(char value) : kind(TerminalKind::Character), value(value) {}

  char Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

  std::string Format() const override { return std::string("'") + value + "'"; }

private:
  TerminalKind kind;
  char value;
};

struct StringTerm : public Terminal {
  explicit StringTerm(std::string value)
      : kind(TerminalKind::String), value(std::move(value)) {}

  std::string Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

  std::string Format() const override {
    auto ret = std::string(this->value);
    replace_all(ret, "\\", "\\\\");
    replace_all(ret, "\"", "\\\"");
    replace_all(ret, "\n", "\\n");
    return std::string("\"") + ret + "\"";
  }

private:
  TerminalKind kind;
  std::string value;
};

struct IdentifierTerm : public Terminal {
  explicit IdentifierTerm(std::string value)
      : kind(TerminalKind::Identifier), value(std::move(value)) {}

  std::string Value() const { return value; }
  TerminalKind Kind() const override { return this->kind; }

  std::string Format() const override { return value; }

private:
  TerminalKind kind;
  std::string value;
};

struct UnitTerm : public Terminal {
  explicit UnitTerm() {}

  TerminalKind Kind() const override { return TerminalKind::UnitVal; }

  std::string Format() const override { return "()"; }
};


struct TerminalNode {
  TerminalNode() = delete;

  template <typename T> explicit TerminalNode(T value) : value(value) {}

  TerminalKind Kind() const { return value->Kind(); }
  auto Value() const { return value; }
  auto Format() const { return value->Format(); }

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

} // namespace aatbe::parser