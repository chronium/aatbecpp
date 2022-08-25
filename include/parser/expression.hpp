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
};

struct Expression {
  Expression() = default;
  Expression(Expression &&) = delete;
  Expression(Expression const &) = delete;
  Expression &operator=(Expression const &) = delete;

  virtual ~Expression() = default;

  virtual ExpressionKind Kind() const = 0;
};

struct AtomExpression : public Expression {
  explicit AtomExpression(const TerminalNode &terminal)
      : terminal(std::make_shared<TerminalNode>(terminal)) {}

  auto Value() const { return this->terminal; }
  ExpressionKind Kind() const { return ExpressionKind::Atom; }

private:
  std::shared_ptr<TerminalNode> terminal;
};

struct UnaryExpression;
struct BinaryExpression;
struct TupleExpression;
struct CallExpression;

struct ExpressionNode {
  ExpressionNode() = delete;

  template <typename T> explicit ExpressionNode(T value) : value(value) {}

  auto Value() const { return value; }
  auto Kind() const { return value->Kind(); }

  auto AsUnary() { return (UnaryExpression *)value; }

  auto AsAtom() { return (AtomExpression *)value; }

  auto AsTuple() { return (TupleExpression *)value; }

  auto AsBinary() { return (BinaryExpression *)value; }

  auto AsCall() { return (CallExpression *)value; }

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
  UnaryExpression(UnaryKind opKind, ExpressionNode expr)
      : opKind(opKind), expr(std::make_shared<ExpressionNode>(expr)) {}

  UnaryKind OpKind() const { return this->opKind; }

  auto Value() const { return this->expr; }
  ExpressionKind Kind() const { return ExpressionKind::Unary; }

private:
  UnaryKind opKind;
  std::shared_ptr<ExpressionNode> expr;
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

  BinaryKind OpKind() const { return this->opKind; }

  auto Value() const { return std::make_tuple(this->left, this->right); }

  auto Left() const { return this->left; }
  auto Right() const { return this->right; }
  ExpressionKind Kind() const { return ExpressionKind::Binary; }

private:
  BinaryKind opKind;
  std::shared_ptr<ExpressionNode> left;
  std::shared_ptr<ExpressionNode> right;
};

struct TupleExpression : public Expression {
  explicit TupleExpression(std::vector<ExpressionNode *> elements)
      : elements(std::move(elements)) {}

  auto Size() const { return this->elements.size(); }
  std::vector<ExpressionNode *> Value() const { return this->elements; }
  ExpressionKind Kind() const { return ExpressionKind::Tuple; }

private:
  std::vector<ExpressionNode *> elements;
};

struct CallExpression : public Expression {
  CallExpression() = delete;
  CallExpression(ExpressionNode *callee, ExpressionNode *args)
      : callee(callee), args(args) {}

  auto Callee() const { return this->callee; }
  auto Args() const { return this->args; }
  ExpressionKind Kind() const { return ExpressionKind::Call; }

private:
  ExpressionNode *callee;
  ExpressionNode *args;
};

ParseResult<ExpressionNode *> ParseExpression(Parser &tokens);

} // namespace aatbe::parser
