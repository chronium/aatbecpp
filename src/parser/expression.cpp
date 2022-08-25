//
// Created by chronium on 24.08.2022.
//

#include <parser/terminal.hpp>
#include <parser/expression.hpp>

#include <vector>

namespace aatbe::parser {

ParseResult<AtomExpression *> ParseAtom(Parser &parser) {
  auto token = ParseTerminal(parser);
  if (!token) {
    return ParserError(ParseErrorKind::ExpectedTerminal, "");
  }

  return ParserSuccess(new AtomExpression(*token.Node()));
}

ParseResult<UnaryExpression *> ParseUnary(Parser &parser) {
  auto token = parser.Read();
  if (!token)
    return ParserError(ParseErrorKind::ExpectedToken, "");

  if (token->get()->Kind() != TokenKind::Symbol)
    return ParserError(ParseErrorKind::ExpectedSymbol, "");

  auto op = token->get()->ValueS();

  if (op == "!") {
    auto expr = ParseExpression(parser);
    if (!expr)
      return ParserError(ParseErrorKind::ExpectedExpression, "");

    return ParserSuccess(new UnaryExpression(
        UnaryExpression::UnaryKind::LogicalNot, *expr.Node()));
  } else if (op == "-") {
    auto expr = ParseExpression(parser);
    if (!expr)
      return ParserError(ParseErrorKind::ExpectedExpression, "");

    return ParserSuccess(new UnaryExpression(
        UnaryExpression::UnaryKind::Negation, *expr.Node()));
  } else if (op == "~") {
    auto expr = ParseExpression(parser);
    if (!expr)
      return ParserError(ParseErrorKind::ExpectedExpression, "");

    return ParserSuccess(new UnaryExpression(
        UnaryExpression::UnaryKind::BitwiseNot, *expr.Node()));
  } else if (op == "&") {
    auto expr = ParseExpression(parser);
    if (!expr)
      return ParserError(ParseErrorKind::ExpectedExpression, "");

    return ParserSuccess(new UnaryExpression(
        UnaryExpression::UnaryKind::AddressOf, *expr.Node()));
  } else if (op == "*") {
    auto expr = ParseExpression(parser);
    if (!expr)
      return ParserError(ParseErrorKind::ExpectedExpression, "");

    return ParserSuccess(new UnaryExpression(
        UnaryExpression::UnaryKind::Dereference, *expr.Node()));
  }

  return ParserError(ParseErrorKind::InvalidOperator, "");
}

ParseResult<TupleExpression *> ParseTuple(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "("))
    return ParserError(ParseErrorKind::ExpectedToken, "");
  std::vector<ExpressionNode *> exprs;
  while (true) {
    auto expr = ParseExpression(parser);
    if (!expr)
      return expr.Error();
    exprs.push_back(expr.Node());
    if (!parser.Read(TokenKind::Symbol, ","))
      break;
  }
  if (!parser.Read(TokenKind::Symbol, ")"))
    return ParserError(ParseErrorKind::ExpectedToken, "");
  return ParserSuccess(new TupleExpression(exprs));
}

ParseResult<ExpressionNode *> ParseExpression(Parser &parser) {
  auto unary = parser.Try(ParseUnary);
  if (unary)
    return ParserSuccess(new ExpressionNode(unary.Node()));

  auto tuple = parser.Try(ParseTuple);
  if (tuple)
    return ParserSuccess(new ExpressionNode(tuple.Node()));

  auto atom = ParseAtom(parser);
  if (!atom)
    return atom.Error();

  auto tupleArgs = parser.Try(ParseTuple);
  if (tupleArgs)
    return ParserSuccess(new ExpressionNode(
        new CallExpression(new ExpressionNode(atom.Node()),
                           new ExpressionNode(tupleArgs.Node()))));

  return ParserSuccess(new ExpressionNode(atom.Node()));
}

} // namespace aatbe::parser
