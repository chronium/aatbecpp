//
// Created by chronium on 16.08.2022.
//

#include <lexer/token.hpp>

namespace aatbe::lexer {
Token::Token(TokenKind kind,
             std::string *valueS,
             uint64_t valueI,
             std::shared_ptr<SrcFile> &file,
             size_t start,
             size_t end)
    : kind(kind),
      valueS(valueS),
      valueI(valueI),
      file(file),
      start(start),
      end(end) {}
}
