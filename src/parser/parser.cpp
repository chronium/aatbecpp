//
// Created by chronium on 18.08.2022.
//

#include <lexer/lexer.hpp>
#include <parser/ast.hpp>
#include <parser/parser.hpp>

#include <parser/atoms.hpp>

#include <vector>

using namespace aatbe::lexer;
using namespace aatbe::parser;

namespace aatbe::parser {

ParseResult<Token *, BooleanAtom> ParseBoolean(std::vector<Token *> tokens) {
  if (tokens.empty()) {
    return ParserError(ParserErrorKind::InvalidToken, "Expected boolean");
  }
  if (tokens[0]->Kind() == TokenKind::Boolean) {
    return ParserSuccess(std::vector<Token *>(tokens.begin() + 1, tokens.end()),
                         BooleanAtom(tokens[0]->ValueI()));
  } else {
    return ParserError(ParserErrorKind::InvalidToken, "Expected boolean");
  }
}

} // namespace aatbe::parser
