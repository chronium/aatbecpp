//
// Created by chronium on 18.08.2022.
//

#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>

using namespace aatbe::lexer;
using namespace aatbe::parser;

namespace aatbe::parser {

auto Parser::PeekKeyword(const char *keyword) {
  if (auto token = this->Peek())
    if (token->get()->Kind() == TokenKind::Keyword)
      return token->get()->ValueS() == keyword;

  return false;
}

auto Parser::ReadKeyword(const char *keyword) {
  return this->PeekKeyword(keyword) ? this->Read() : std::nullopt;
}

ParseResult<ModuleStatement> Parser::ParseModuleStatement() {
  if (ReadKeyword("fn"))
    return ParserSuccess(ModuleStatement(FunctionStatement("")));

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<TerminalNode> Parser::ParseTerminal() {
  if (auto token = this->Peek()) {
    switch (token->get()->Kind()) {
    case TokenKind::Number:
      return ParserSuccess(TerminalNode(IntegerTerm(this->Read()->get()->ValueI())));
    case TokenKind::Char:
      return ParserSuccess(TerminalNode(CharTerm(this->Read()->get()->ValueS()[0])));
    case TokenKind::String:
      return ParserSuccess(TerminalNode(StringTerm(this->Read()->get()->ValueS())));
    case TokenKind::Boolean:
      return ParserSuccess(TerminalNode(BooleanTerm(this->Read()->get()->ValueI())));
    default:
      return ParserError(ParseErrorKind::InvalidToken, "");
    }
  }
  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

} // namespace aatbe::parser
