#pragma once

#include <string>
#include <utility>
#include <vector>
#include <optional>

#include <parser/bindings.hpp>
#include <parser/expression.hpp>
#include <parser/type.hpp>

#include <codegen.hpp>

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
                    TypeNode *returnType, std::optional<ExpressionNode *> body,
                    bool isVariadic)
      : isExtern(isExtern), name(std::move(name)), parameters(parameters),
        returnType(returnType), body(body), isVariadic(isVariadic) {}

  auto Name() const { return this->name; }
  auto Parameters() const { return this->parameters; }
  auto ReturnType() const { return this->returnType; }
  auto Body() const { return this->body; }
  auto IsExtern() const { return this->isExtern; }
  auto IsVariadic() const { return this->isVariadic; }

  auto Value() const { return std::make_tuple(name, parameters, returnType); }

  ModuleStatementKind Kind() const override {
    return ModuleStatementKind::Function;
  }

  llvm::FunctionType *Type() {
    std::vector<llvm::Type *> paramTypes;
    for (auto &param : parameters->Bindings()) {
      paramTypes.push_back(param->Type()->LLVMType());
    }
    return llvm::FunctionType::get(returnType->LLVMType(), paramTypes, false);
  }

  std::string Format() const override {
    std::string ext = isExtern ? "Extern" : "";
    auto variadic = isVariadic ? "Variadic" : "";
    auto res = ext + variadic + "Function(" + name + ", args " +
               parameters->Format() + ", ret " + returnType->Format() + ")";

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
  bool isVariadic;
};

struct StructStatement : ModuleStatement {
  StructStatement() = delete;

  StructStatement(std::string name, MemberList *members)
      : name(std::move(name)), members(members) {}

  auto Name() const { return this->name; }
  auto Members() const { return this->members; }

  auto Value() const { return std::make_tuple(name, members); }

  ModuleStatementKind Kind() const override {
    return ModuleStatementKind::Struct;
  }

  std::string Format() const override {
    return "Struct(" + name + ", " + members->Format() + ")";
  }

  llvm::Type *LLVMType() {
    std::vector<llvm::Type *> memberTypes;
    for (auto &member : members->Bindings()) {
      memberTypes.push_back(member->Type()->LLVMType());
    }
    return llvm::StructType::create(*LLVMContext, memberTypes, this->name);
  }

private:
  std::string name;
  MemberList *members;
};

struct ModuleStatementNode {
  ModuleStatementNode() = delete;

  template <typename T> explicit ModuleStatementNode(T value) : value(value) {}

  auto Value() { return value; }
  auto Kind() { return value->Kind(); }
  auto Format() { return value->Format(); }

  auto AsFunction() { return (FunctionStatement *)value; }
  auto AsStruct() { return (StructStatement *)value; }

  std::string Format() const { return value->Format(); }

private:
  ModuleStatement *value;
};

struct ModuleNode {
public:
  explicit ModuleNode(std::vector<ModuleStatementNode *> statements)
      : statements(std::move(statements)) {}

  auto Value() const { return this->statements; }
  auto Kind() const { return ModuleStatementKind::Module; }
  std::string Format() const {
    std::string res = "Module(";

    for (auto statement : statements) {
      res += statement->Format() + ", ";
    }

    res += ")";

    return res;
  }

private:
  std::string name;

  std::vector<ModuleStatementNode *> statements;
};

} // namespace aatbe::parser
