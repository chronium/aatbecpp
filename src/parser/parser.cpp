//
// Created by chronium on 18.08.2022.
//

#include <memory>

#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>

using namespace aatbe::lexer;

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

std::optional<std::reference_wrapper<Token *>>
Parser::ReadKeyword(const char *keyword) {
  return this->PeekKeyword(keyword) ? this->Read() : std::nullopt;
}

ParseResult<IdentifierTerm *> ParseIdentifier(Parser &parser) {
  if (auto token = parser.Peek()) {
    if (token->get()->Kind() == TokenKind::Identifier)
      return ParserSuccess(new IdentifierTerm(parser.Read()->get()->ValueS()));
  }
  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<IntegerTerm *> ParseNumber(Parser &parser) {
  if (auto token = parser.Peek()) {
    if (token->get()->Kind() == TokenKind::Number) {
      parser.Read();
      if (auto type = ParseType(parser))
        return ParserSuccess(
            new IntegerTerm(token->get()->ValueI(), type.Node()));
      else
        return ParserSuccess(
            new IntegerTerm(token->get()->ValueI(),
                            new TypeNode(new SIntType(TypeKind::Int64))));
    }
  }
  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<TerminalNode *> ParseTerminal(Parser &parser) {
  if (auto token = parser.Peek()) {
    switch (token->get()->Kind()) {
    case TokenKind::Number:
      return ParseNumber(parser).WrapWith<TerminalNode>();
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

ParseResult<ModuleNode *> Parser::Parse() {
  auto statements = std::vector<ModuleStatementNode *>();

  while (this->Peek()) {
    if (auto statement = ParseModuleStatement(*this))
      statements.push_back(statement.Node());
    else
      return ParserError(ParseErrorKind::UnexpectedToken, "");
  }

  return ParserSuccess(new ModuleNode(statements));
}

void replace_all(std::string &s, std::string const &toReplace,
                 std::string const &replaceWith) {
  std::string buf;
  std::size_t pos = 0;
  std::size_t prevPos;

  // Reserves rough estimate of final size of string.
  buf.reserve(s.size());

  while (true) {
    prevPos = pos;
    pos = s.find(toReplace, pos);
    if (pos == std::string::npos)
      break;
    buf.append(s, prevPos, pos - prevPos);
    buf += replaceWith;
    pos += toReplace.size();
  }

  buf.append(s, prevPos, s.size() - prevPos);
  s.swap(buf);
}

} // namespace aatbe::parser
