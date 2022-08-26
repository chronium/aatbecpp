//
// Created by chronium on 22.08.2022.
//

#include <parser/parser.hpp>
#include <parser/terminal.hpp>
#include <parser/type.hpp>

#include <variant>

namespace aatbe::parser {

ParseResult<SIntType *> ParseSInt(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "int8"))
    return ParserSuccess(new SIntType(TypeKind::Int8));
  if (parser.Read(TokenKind::Identifier, "int16"))
    return ParserSuccess(new SIntType(TypeKind::Int16));
  if (parser.Read(TokenKind::Identifier, "int32"))
    return ParserSuccess(new SIntType(TypeKind::Int32));
  if (parser.Read(TokenKind::Identifier, "int64"))
    return ParserSuccess(new SIntType(TypeKind::Int64));

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<UIntType *> ParseUInt(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "uint8"))
    return ParserSuccess(new UIntType(TypeKind::UInt8));
  if (parser.Read(TokenKind::Identifier, "uint16"))
    return ParserSuccess(new UIntType(TypeKind::UInt16));
  if (parser.Read(TokenKind::Identifier, "uint32"))
    return ParserSuccess(new UIntType(TypeKind::UInt32));
  if (parser.Read(TokenKind::Identifier, "uint64"))
    return ParserSuccess(new UIntType(TypeKind::UInt64));

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<FloatType *> ParseFloat(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "float32"))
    return ParserSuccess(new FloatType(TypeKind::Float32));
  if (parser.Read(TokenKind::Identifier, "float64"))
    return ParserSuccess(new FloatType(TypeKind::Float64));

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<BoolType *> ParseBool(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "bool"))
    return ParserSuccess(new BoolType());

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<CharType *> ParseChar(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "char"))
    return ParserSuccess(new CharType());

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<StrType *> ParseString(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "str"))
    return ParserSuccess(new StrType());

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<SliceType *> ParseSlice(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "["))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  ErrorOrContinue(type, ParseType(parser));

  if (!parser.Read(TokenKind::Symbol, "]"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  return type.WrapWith<SliceType>();
}

ParseResult<ArrayType *> ParseArray(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "["))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  ErrorOrContinue(type, ParseType(parser));

  if (!parser.Read(TokenKind::Symbol, ";"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  ErrorOrContinue(size, ParseTerminal(parser));

  if (size.Kind() != TerminalKind::Integer)
    return ParserError(ParseErrorKind::ExpectedToken, "");

  if (!parser.Read(TokenKind::Symbol, "]"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  return type.WrapWith<ArrayType>(Unwrap(size, Integer));
}

ParseResult<RefType *> ParseRef(Parser &parser) {
  if (!parser.Read(TokenKind::Keyword, "ref"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  TryReturnOrError(ParseType(parser).WrapWith<RefType>());
}

ParseResult<PointerType *> ParsePointer(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "*"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  TryReturnOrError(ParseType(parser).WrapWith<PointerType>());
}

ParseResult<TypeNode *> ParseType(Parser &parser) {
  TryReturn(parser.Try(ParseSInt).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseUInt).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseFloat).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseBool).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseChar).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseString).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseSlice).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseArray).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParseRef).WrapWith<TypeNode>());
  TryReturn(parser.Try(ParsePointer).WrapWith<TypeNode>());

  return ParserError(ParseErrorKind::ExpectedType, "");
}

} // namespace aatbe::parser
