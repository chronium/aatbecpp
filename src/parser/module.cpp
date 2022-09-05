//
// Created by chronium on 05.09.2022.
//

#include <parser/parser.hpp>
#include <parser/ast.hpp>

namespace aatbe::parser {

ParseResult<FunctionStatement *> ParseFunction(Parser &parser) {
  auto isExtern = parser.ReadKeyword("extern").has_value();

  if (!parser.Read(TokenKind::Keyword, "fn"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  ErrorOrContinue(name, ParseIdentifier(parser));

  auto argList = Deffer(new ParameterList(parser.DelimitedBy(
      [](Parser &parser) {
        if (auto param = ParseParameter(parser))
          return param.Node();
        else
          return (ParameterBinding *)nullptr;
      },
      TokenKind::Symbol, ",")));

  auto isVariadic = false;
  ParseResult<ParameterList *> args =
      ParserSuccess(new ParameterList(std::vector<ParameterBinding *>{}));
  if (parser.Peek("(", 0) && parser.Peek(")", 1)) {
    parser.Read();
    parser.Read();
  } else if (!parser.Peek(TokenKind::Symbol, "->") &&
             !parser.Peek(TokenKind::Symbol, "=") && parser.Peek()) {
    args = parser.SurroundedBy(
        [argList, &isVariadic](Parser &parser) {
          auto list = argList(parser);
          isVariadic = parser.Read("...").has_value();
          return list;
        },
        TokenKind::Symbol, "(", TokenKind::Symbol, ")");
  }

  auto returnType = new TypeNode(new UnitType());

  if (parser.Read(TokenKind::Symbol, "->")) {
    if (auto type = ParseType(parser))
      returnType = type.Node();
    else
      return ParserError(ParseErrorKind::ExpectedType, "");
  }

  auto body = parser.Read(TokenKind::Symbol, "=")
                  ? std::optional(ParseExpression(parser).Node())
                  : std::nullopt;

  return ParserSuccess(new FunctionStatement(isExtern, name.Node()->Value(),
                                             args.Node(), returnType, body,
                                             isVariadic));
}

ParseResult<ModuleStatementNode *> ParseModuleStatement(Parser &parser) {
  TryReturn(parser.Try(ParseFunction).WrapWith<ModuleStatementNode>());
  TryReturn(parser.Try(ParseStruct).WrapWith<ModuleStatementNode>())

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<StructStatement *> ParseStruct(Parser &parser) {
  if (!parser.Read(TokenKind::Keyword, "struct"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  ErrorOrContinue(name, ParseIdentifier(parser));
  ErrorOrContinue(members, ParseMemberList(parser));

  return ParserSuccess(new StructStatement(name.Node()->Value(), members.Node()));
}

} // namespace aatbe::parser