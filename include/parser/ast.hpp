#pragma once

#include <string>
#include <utility>
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
  ModuleStatement() = default;
  ModuleStatement(ModuleStatement &&) = delete;
  ModuleStatement(ModuleStatement const &) = delete;
  ModuleStatement &operator=(ModuleStatement const &) = delete;

  virtual ~ModuleStatement() = default;

  virtual ModuleStatementKind Kind() const = 0;
};

struct FunctionStatement : ModuleStatement {
  FunctionStatement() = delete;

  explicit FunctionStatement(std::string name) : name(std::move(name)) {}

  auto Name() const { return this->name; }
  ModuleStatementKind Kind() const override {
    return ModuleStatementKind::Function;
  }

private:
  std::string name;
};

struct ModuleStatementNode {
  ModuleStatementNode() = delete;

  template <typename T> explicit ModuleStatementNode(T value) : value(value) {}

  auto Value() { return value; }
  auto Kind() { return value->Kind(); }

  auto AsFunction() { return (FunctionStatement *)value; }

private:
  ModuleStatement *value;
};

struct ModuleNode {
public:
  explicit ModuleNode(std::vector<std::unique_ptr<ModuleStatementNode>> statements)
      : statements(std::move(statements)) {}

private:
  std::string name;

  std::vector<std::unique_ptr<ModuleStatementNode>> statements;
};

} // namespace aatbe::parser
