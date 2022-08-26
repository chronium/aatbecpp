#pragma once

#include <string>
#include <utility>
#include <vector>
#include <optional>

#include <parser/type.hpp>
#include <parser/parameters.hpp>
#include <parser/expression.hpp>

namespace aatbe::parser {

struct ModuleStatement {
  ModuleStatement() = default;
  ModuleStatement(ModuleStatement &&) = delete;
  ModuleStatement(ModuleStatement const &) = delete;
  ModuleStatement &operator=(ModuleStatement const &) = delete;

  virtual ~ModuleStatement() = default;

  virtual ModuleStatementKind Kind() const = 0;
  virtual std::string Format() const = 0;
};

struct FunctionStatement : ModuleStatement {
  FunctionStatement() = delete;

  FunctionStatement(bool isExtern, std::string name, ParameterList *parameters,
                    TypeNode *returnType, std::optional<ExpressionNode *> body)
      : isExtern(isExtern), name(std::move(name)), parameters(parameters),
        returnType(returnType), body(body) {}

  auto Name() const { return this->name; }
  auto Parameters() const { return this->parameters; }
  auto ReturnType() const { return this->returnType; }
  auto Body() const { return this->body; }
  auto IsExtern() const { return this->isExtern; }

  auto Value() const { return std::make_tuple(name, parameters, returnType); }

  ModuleStatementKind Kind() const override {
    return ModuleStatementKind::Function;
  }

  std::string Format() const override {
    auto res = "Function(" + name + ", args " + parameters->Format() +
               ", ret " + returnType->Format() + ")";

    if (body.has_value()) {
      res += " = " + body.value()->Format();
    }

    return res;
  }

private:
  bool isExtern;
  std::string name;
  ParameterList *parameters;
  TypeNode *returnType;
  std::optional<ExpressionNode *> body;
};

struct ModuleStatementNode {
  ModuleStatementNode() = delete;

  template <typename T> explicit ModuleStatementNode(T value) : value(value) {}

  auto Value() { return value; }
  auto Kind() { return value->Kind(); }
  auto Format() { return value->Format(); }

  auto AsFunction() { return (FunctionStatement *)value; }

  std::string Format() const { return value->Format(); }

private:
  ModuleStatement *value;
};

struct ModuleNode {
public:
  explicit ModuleNode(
      std::vector<std::unique_ptr<ModuleStatementNode>> statements)
      : statements(std::move(statements)) {}

private:
  std::string name;

  std::vector<std::unique_ptr<ModuleStatementNode>> statements;
};

} // namespace aatbe::parser
