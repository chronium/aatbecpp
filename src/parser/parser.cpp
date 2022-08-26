//
// Created by chronium on 18.08.2022.
//

#include <cassert>

#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>

using namespace aatbe::lexer;
using namespace aatbe::parser;

namespace aatbe::parser {

bool Parser::Match(TokenKind kind) {
  if (auto token = this->Peek())
    return token->get()->Kind() == kind;

  return false;
}

auto Parser::PeekKeyword(const char *keyword) {
  if (auto token = this->Peek())
    if (token->get()->Kind() == TokenKind::Keyword)
      return token->get()->ValueS() == keyword;

  return false;
}

auto Parser::ReadKeyword(const char *keyword) {
  return this->PeekKeyword(keyword) ? this->Read() : std::nullopt;
}

ParseResult<ParameterBinding *> ParseParameter(Parser &parser) {
  if (auto token = parser.Peek()) {
    if (token->get()->Kind() == TokenKind::Identifier) {
      auto name = parser.Read()->get()->ValueS();
      if (parser.Read(TokenKind::Symbol, ":")) {
        if (auto type = ParseType(parser))
          return type.WrapWith<ParameterBinding>(name);
      }
    }
  }
  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<IdentifierTerm *> ParseIdentifier(Parser &parser) {
  if (auto token = parser.Peek()) {
    if (token->get()->Kind() == TokenKind::Identifier)
      return ParserSuccess(new IdentifierTerm(parser.Read()->get()->ValueS()));
  }
  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<FunctionStatement *> ParseFunction(Parser &parser) {
  auto isExtern = parser.ReadKeyword("extern").has_value();

  if (!parser.Read(TokenKind::Keyword, "fn"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  ErrorOrContinue(name, ParseIdentifier(parser));

  auto argList = Deffer(new ParameterList(parser.DelimitedBy(
      Deffer(ParseParameter(parser).Node()), TokenKind::Symbol, ",")));

  ParseResult<ParameterList *> args =
      ParserSuccess(new ParameterList(std::vector<ParameterBinding *>{}));
  if (parser.Peek("(", 0) && parser.Peek(")", 1)) {
    parser.Read();
    parser.Read();
  } else if (!parser.Peek(TokenKind::Symbol, "->") &&
             !parser.Peek(TokenKind::Symbol, "=") && parser.Peek())
    args = parser.SurroundedBy(argList, TokenKind::Symbol, "(",
                               TokenKind::Symbol, ")");

  auto returnType = new TypeNode(new UnitType());

  if (parser.Read(TokenKind::Symbol, "->")) {
    if (auto type = ParseType(parser))
      returnType = type.Node();
    else
      return ParserError(ParseErrorKind::ExpectedType, "");
  }

  auto body = parser.Read(TokenKind::Symbol, "=") ? std::optional(ParseExpression(parser).Node()) : std::nullopt;

  return ParserSuccess(new FunctionStatement(
      isExtern, name.Node()->Value(), args.Node(), returnType, body));
}

ParseResult<ModuleStatementNode *> ParseModuleStatement(Parser &parser) {
  TryReturn(parser.Try(ParseFunction).WrapWith<ModuleStatementNode>());

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<TerminalNode *> ParseTerminal(Parser &parser) {
  if (auto token = parser.Peek()) {
    switch (token->get()->Kind()) {
    case TokenKind::Number:
      return ParserSuccess(
          new TerminalNode(new IntegerTerm(parser.Read()->get()->ValueI())));
    case TokenKind::Char:
      return ParserSuccess(
          new TerminalNode(new CharTerm(parser.Read()->get()->ValueS()[0])));
    case TokenKind::String:
      return ParserSuccess(
          new TerminalNode(new StringTerm(parser.Read()->get()->ValueS())));
    case TokenKind::Boolean:
      return ParserSuccess(
          new TerminalNode(new BooleanTerm(parser.Read()->get()->ValueI())));
    case TokenKind::Identifier:
      return ParserSuccess(
          new TerminalNode(new IdentifierTerm(parser.Read()->get()->ValueS())));
    default:
      if (parser.Peek("(", 0) && parser.Peek(")", 1)) {
        parser.Read();
        parser.Read();
        return ParserSuccess(new TerminalNode(new UnitTerm()));
      }
      return ParserError(ParseErrorKind::InvalidToken, "");
    }
  }
  return ParserError(ParseErrorKind::UnexpectedEof, "");
}

} // namespace aatbe::parser
