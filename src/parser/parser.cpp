//
// Created by chronium on 18.08.2022.
//

#include <cassert>

#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <parser/terminal.hpp>

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

ParseResult<ModuleStatementNode *> Parser::ParseModuleStatement() {
  if (ReadKeyword("fn"))
    return ParserSuccess(new ModuleStatementNode(new FunctionStatement("")));

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
      return ParserError(ParseErrorKind::InvalidToken, "");
    }
  }
  return ParserError(ParseErrorKind::UnexpectedEof, "");
}

} // namespace aatbe::parser
