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

struct ModuleStatementNode {
  ModuleStatementNode() = delete;

  explicit ModuleStatementNode(ModuleStatementKind kind) : kind(kind) {}

  auto Kind() const { return this->kind; }

private:
  ModuleStatementKind kind;
};

struct FunctionStatement : ModuleStatementNode {
  FunctionStatement() = delete;

  explicit FunctionStatement(std::string name)
      : ModuleStatementNode(ModuleStatementKind::Function),
        name(std::move(name)) {}

  auto Name() const { return this->name; }

private:
  std::string name;
};

struct ModuleStatement {
  ModuleStatement() = delete;

  template <typename T>
  explicit ModuleStatement(T value)
      : value(std::make_shared<T>(std::move(value))) {}

  auto Value() { return value; }
  auto Kind() { return value->Kind(); }

  auto AsFunction() {
    return std::static_pointer_cast<FunctionStatement>(value);
  }

private:
  std::shared_ptr<ModuleStatementNode> value;
};

struct ModuleNode {
public:
  explicit ModuleNode(std::vector<std::unique_ptr<ModuleStatement>> statements)
      : statements(std::move(statements)) {}

private:
  std::string name;

  std::vector<std::unique_ptr<ModuleStatement>> statements;
};

} // namespace aatbe::parser
