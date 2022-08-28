//
// Created by chronium on 24.08.2022.
//

#pragma once

#include <parser/terminal.hpp>

#include <vector>
#include <memory>

namespace aatbe::parser {
enum ExpressionKind : int {
  Unary,
  Binary,
  Atom,
  Call,
  Tuple,
  Block,
};

struct Expression {
  Expression() = default;
  Expression(Expression &&) = delete;
  Expression(Expression const &) = delete;
  Expression &operator=(Expression const &) = delete;

  virtual ~Expression() = default;

  virtual ExpressionKind Kind() const = 0;
  virtual std::string Format() const = 0;
};

struct AtomExpression : public Expression {
  explicit AtomExpression(const TerminalNode *terminal) : terminal(terminal) {}

  auto Value() const { return this->terminal; }
  ExpressionKind Kind() const override { return ExpressionKind::Atom; }

  std::string Format() const override { return this->terminal->Format(); }

private:
  const TerminalNode *terminal;
};

struct UnaryExpression;
struct BinaryExpression;
struct TupleExpression;
struct CallExpression;
struct BlockExpression;

struct ExpressionNode {
  ExpressionNode() = delete;

  template <typename T> explicit ExpressionNode(T value) : value(value) {}

  auto Value() const { return value; }
  auto Kind() const { return value->Kind(); }
  auto Format() const { return value->Format(); }

  auto AsUnary() { return (UnaryExpression *)value; }

  auto AsAtom() { return (AtomExpression *)value; }

  auto AsTuple() { return (TupleExpression *)value; }

  auto AsBinary() { return (BinaryExpression *)value; }

  auto AsCall() { return (CallExpression *)value; }

  auto AsBlock() { return (BlockExpression *)value; }

  auto Node() const { return this; }

private:
  Expression *value;
};

struct UnaryExpression : public Expression {
  enum UnaryKind : int {
    Negation,
    LogicalNot,
    BitwiseNot,
    AddressOf,
    Dereference,
  };

  UnaryExpression() = delete;
  UnaryExpression(const ExpressionNode *expr, UnaryKind opKind)
      : opKind(opKind), expr(expr) {}

  std::string Format() const override {
    switch (opKind) {
    case UnaryKind::Negation:
      return "-" + expr->Format();
    case UnaryKind::LogicalNot:
      return "!" + expr->Format();
    case UnaryKind::BitwiseNot:
      return "~" + expr->Format();
    case UnaryKind::AddressOf:
      return "&" + expr->Format();
    case UnaryKind::Dereference:
      return "*" + expr->Format();
    default:
      return "unknown unop";
    }
  }

  UnaryKind OpKind() const { return this->opKind; }

  auto Value() const { return this->expr; }
  ExpressionKind Kind() const override { return ExpressionKind::Unary; }

private:
  UnaryKind opKind;
  const ExpressionNode *expr;
};

struct BinaryExpression : public Expression {
  enum BinaryKind : int {
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Modulo,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseLeftShift,
    BitwiseRightShift,
    LogicalAnd,
    LogicalOr,
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
  };

  BinaryExpression() = delete;
  BinaryExpression(std::shared_ptr<ExpressionNode> left,
                   std::shared_ptr<ExpressionNode> right)
      : left(std::move(left)), right(std::move(right)) {}

  std::string Format() const override {
    switch (opKind) {
    case BinaryKind::Addition:
      return left->Format() + " + " + right->Format();
    case BinaryKind::Subtraction:
      return left->Format() + " - " + right->Format();
    case BinaryKind::Multiplication:
      return left->Format() + " * " + right->Format();
    case BinaryKind::Division:
      return left->Format() + " / " + right->Format();
    case BinaryKind::Modulo:
      return left->Format() + " % " + right->Format();
    case BinaryKind::BitwiseAnd:
      return left->Format() + " & " + right->Format();
    case BinaryKind::BitwiseOr:
      return left->Format() + " | " + right->Format();
    case BinaryKind::BitwiseXor:
      return left->Format() + " ^ " + right->Format();
    case BinaryKind::BitwiseLeftShift:
      return left->Format() + " << " + right->Format();
    case BinaryKind::BitwiseRightShift:
      return left->Format() + " >> " + right->Format();
    case BinaryKind::LogicalAnd:
      return left->Format() + " && " + right->Format();
    case BinaryKind::LogicalOr:
      return left->Format() + " || " + right->Format();
    case BinaryKind::Equal:
      return left->Format() + " == " + right->Format();
    case BinaryKind::NotEqual:
      return left->Format() + " != " + right->Format();
    case BinaryKind::LessThan:
      return left->Format() + " < " + right->Format();
    case BinaryKind::LessThanOrEqual:
      return left->Format() + " <= " + right->Format();
    case BinaryKind::GreaterThan:
      return left->Format() + " > " + right->Format();
    case BinaryKind::GreaterThanOrEqual:
      return left->Format() + " >= " + right->Format();
    default:
      return "unknown binop";
    }
  }

  BinaryKind OpKind() const { return this->opKind; }

  auto Value() const { return std::make_tuple(this->left, this->right); }

  auto Left() const { return this->left; }
  auto Right() const { return this->right; }
  ExpressionKind Kind() const override { return ExpressionKind::Binary; }

private:
  BinaryKind opKind;
  std::shared_ptr<ExpressionNode> left;
  std::shared_ptr<ExpressionNode> right;
};

struct TupleExpression : public Expression {
  explicit TupleExpression(std::vector<ExpressionNode *> elements)
      : elements(std::move(elements)) {}

  std::string Format() const override {
    std::string result = "(";
    for (auto element : elements) {
      result += element->Format() + ", ";
    }
    result += ")";
    return result;
  }

  auto Size() const { return this->elements.size(); }
  std::vector<ExpressionNode *> Value() const { return this->elements; }
  ExpressionKind Kind() const override { return ExpressionKind::Tuple; }

private:
  std::vector<ExpressionNode *> elements;
};

struct CallExpression : public Expression {
  CallExpression() = delete;
  CallExpression(ExpressionNode *callee, ExpressionNode *args)
      : callee(callee), args(args) {}

  std::string Format() const override {
    return callee->Format() + "(" + args->Format() + ")";
  }

  auto Callee() const { return this->callee; }
  auto Args() const { return this->args; }
  auto Value() const { return std::make_tuple(this->callee, this->args); }
  ExpressionKind Kind() const override { return ExpressionKind::Call; }

private:
  ExpressionNode *callee;
  ExpressionNode *args;
};

struct BlockExpression : public Expression {
  BlockExpression() = delete;
  BlockExpression(std::vector<ExpressionNode *> statements)
      : statements(std::move(statements)) {}

  std::string Format() const override {
    std::string result = "{";
    for (auto statement : statements) {
      result += statement->Format() + "; ";
    }
    result += "}";
    return result;
  }

  auto Value() const { return this->statements; }
  auto Size() const { return this->statements.size(); }
  ExpressionKind Kind() const override { return ExpressionKind::Block; }

private:
  std::vector<ExpressionNode *> statements;
};

} // namespace aatbe::parser
