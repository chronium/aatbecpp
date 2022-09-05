//
// Created by chronium on 05.09.2022.
//

#include <parser/parser.hpp>

namespace aatbe::parser {

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

ParseResult<MemberBinding *> ParseMember(Parser &parser) {
  auto isPublic = parser.ReadKeyword("public").has_value();

  if (auto token = parser.Peek()) {
    if (token->get()->Kind() == TokenKind::Identifier) {
      auto name = parser.Read()->get()->ValueS();
      if (parser.Read(TokenKind::Symbol, ":")) {
        if (auto type = ParseType(parser))
          return type.WrapWith<MemberBinding>(
              name, isPublic ? VisibilityModifier::Public
                             : VisibilityModifier::Internal);
      }
    }
  }
  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<MemberList *> ParseMemberList(Parser &parser) {
  return parser.SurroundedBy(Deffer(new MemberList(parser.DelimitedBy(
                                 [](Parser &parser) {
                                   if (auto param = ParseMember(parser))
                                     return param.Node();
                                   else
                                     return (MemberBinding *)nullptr;
                                 },
                                 TokenKind::Symbol, ";"))),
                             TokenKind::Symbol, "{", TokenKind::Symbol, "}");
}

} // namespace aatbe::parser
