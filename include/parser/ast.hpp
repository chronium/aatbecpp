#pragma once

#include <string>
#include <vector>

namespace aatbe::parser {

enum class ModuleStatementKind {
  Import,
  Export,
  Function,
  Global,
  Constant,
  Structure,
  Typename,
  Type,
};

struct ModuleStatement {
  ModuleStatementKind kind;
};

struct ModuleNode {

private:
  std::string name;

  std::vector<ModuleStatement> statements;
};

enum AtomKind : int {
  Boolean,
};

struct Atom {
  Atom() = delete;

  explicit Atom(AtomKind kind) : kind(kind) {}

  AtomKind Kind() const { return this->kind; }

  AtomKind kind;
};

struct BooleanAtom : Atom {
  explicit BooleanAtom(bool value) : Atom(AtomKind::Boolean), value(value) {}

  bool Value() const { return value; }

  bool value;
};

} // namespace aatbe::parser
