//
// Created by chronium on 24.08.2022.
//

#include <parser/terminal.hpp>
#include <parser/expression.hpp>
#include <parser/parser.hpp>

#include <vector>

namespace aatbe::parser {

ParseResult<AtomExpression *> ParseAtom(Parser &parser) {
  TryReturnOrError(ParseTerminal(parser).WrapWith<AtomExpression>());
}

ParseResult<UnaryExpression *> ParseUnary(Parser &parser) {
  auto token = parser.Read();
  if (!token)
    return ParserError(ParseErrorKind::ExpectedToken, "");

  if (token->get()->Kind() != TokenKind::Symbol)
    return ParserError(ParseErrorKind::ExpectedSymbol, "");

  auto op = token->get()->ValueS();
  ErrorOrContinue(expr, ParseExpression(parser));

  if (op == "!")
    return expr.WrapWith<UnaryExpression>(
        UnaryExpression::UnaryKind::LogicalNot);
  else if (op == "-")
    return expr.WrapWith<UnaryExpression>(UnaryExpression::UnaryKind::Negation);
  else if (op == "~")
    return expr.WrapWith<UnaryExpression>(
        UnaryExpression::UnaryKind::BitwiseNot);
  else if (op == "&")
    return expr.WrapWith<UnaryExpression>(
        UnaryExpression::UnaryKind::AddressOf);
  else if (op == "*")
    return expr.WrapWith<UnaryExpression>(
        UnaryExpression::UnaryKind::Dereference);

  return ParserError(ParseErrorKind::InvalidOperator, "");
}

ParseResult<TupleExpression *> ParseTuple(Parser &parser) {
  auto exprs = Deffer(new TupleExpression(parser.DelimitedBy(
      Deffer(ParseExpression(parser).Node()), TokenKind::Symbol, ",")));

  return parser.SurroundedBy(exprs, TokenKind::Symbol, "(", TokenKind::Symbol,
                             ")");
}

ParseResult<AtomExpression *> ParseUnitAtom(Parser &parser) {
  if (parser.Read(TokenKind::Symbol, "(")) {
    if (parser.Read(TokenKind::Symbol, ")"))
      return ParserSuccess(
          new AtomExpression(new TerminalNode(new UnitTerm())));
    else
      return ParserError(ParseErrorKind::ExpectedToken, "");
  }

  return ParserError(ParseErrorKind::ExpectedToken, "");
}

ParseResult<ExpressionNode *> ParseExpression(Parser &parser) {
  TryReturn(parser.Try(ParseUnary).WrapWith<ExpressionNode>());
  TryReturn(parser.Try(ParseUnitAtom).WrapWith<ExpressionNode>());
  TryReturn(parser.Try(ParseTuple).WrapWith<ExpressionNode>());

  ErrorOrContinue(atom, ParseAtom(parser).WrapWith<ExpressionNode>());

  auto tupleArgs = parser.Try(ParseTuple).WrapWith<ExpressionNode>();
  if (tupleArgs)
    return atom.WrapWith<CallExpression>(tupleArgs.Node())
        .WrapWith<ExpressionNode>();

  return atom;
}

} // namespace aatbe::parser
