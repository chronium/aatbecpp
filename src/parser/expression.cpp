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

ParseResult<BlockExpression *> ParseBlock(Parser &parser) {
  auto exprs = [](Parser &parser) {
    std::vector<ExpressionNode *> vec;
    while (auto expr = ParseExpression(parser)) {
      vec.push_back(expr.Node());
      parser.Read(TokenKind::Symbol, ";");
    }
    return new BlockExpression(vec);
  };
  return parser.SurroundedBy(exprs, TokenKind::Symbol, "{", TokenKind::Symbol,
                             "}");
}

ParseResult<IfExpression *> ParseIf(Parser &parser) {
  if (!parser.Read(TokenKind::Keyword, "if"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  std::vector<std::tuple<ExpressionNode *, ExpressionNode *>> branches;

  ErrorOrContinue(if_cond, ParseExpression(parser));
  parser.Read(TokenKind::Keyword, "then"); // `then` optional after if condition
  ErrorOrContinue(if_body, ParseExpression(parser));

  // First if clause
  branches.emplace_back(if_cond.Node(), if_body.Node());

  // Else if clauses
  while (parser.Read(TokenKind::Keyword, "else")) {
    if (parser.Read(TokenKind::Keyword, "if")) {
      ErrorOrContinue(else_if_cond, ParseExpression(parser));
      parser.Read(TokenKind::Keyword, "then"); // `then` optional after if condition
      ErrorOrContinue(else_if_body, ParseExpression(parser));
      branches.emplace_back(else_if_cond.Node(), else_if_body.Node());
    } else {
      ErrorOrContinue(else_body, ParseExpression(parser));
      branches.emplace_back(nullptr, else_body.Node());
      break;
    }
  }

  return ParserSuccess(new IfExpression(branches));
}

ParseResult<ExpressionNode *> ParseExpression(Parser &parser) {
  TryReturn(parser.Try(ParseUnary).WrapWith<ExpressionNode>());
  TryReturn(parser.Try(ParseUnitAtom).WrapWith<ExpressionNode>());
  TryReturn(parser.Try(ParseTuple).WrapWith<ExpressionNode>());
  TryReturn(parser.Try(ParseBlock).WrapWith<ExpressionNode>());
  TryReturn(parser.Try(ParseIf).WrapWith<ExpressionNode>());

  ErrorOrContinue(atom, ParseAtom(parser).WrapWith<ExpressionNode>());

  auto tupleArgs = parser.Try(ParseTuple).WrapWith<ExpressionNode>();
  if (tupleArgs)
    return atom.WrapWith<CallExpression>(tupleArgs.Node())
        .WrapWith<ExpressionNode>();

  return atom;
}

} // namespace aatbe::parser
