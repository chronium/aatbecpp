//
// Created by chronium on 18.08.2022.
//

#include <lexer/token.hpp>

#include <parser/ast.hpp>
#include <parser/parser.hpp>
#include <vector>

using namespace aatbe::lexer;

namespace aatbe::parser {

ParseResult<Token *, BooleanAtom> ParseBoolean(std::vector<Token *> tokens);

} // namespace aatbe::parser