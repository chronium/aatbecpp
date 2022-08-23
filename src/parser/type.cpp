//
// Created by chronium on 22.08.2022.
//

#include <parser/type.hpp>
#include <parser/parser.hpp>
#include <parser/terminals.hpp>

namespace aatbe::parser {

ParseResult<SIntType> ParseSInt(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "int8"))
    return ParserSuccess(SIntType(TypeKind::Int8));
  if (parser.Read(TokenKind::Identifier, "int16"))
    return ParserSuccess(SIntType(TypeKind::Int16));
  if (parser.Read(TokenKind::Identifier, "int32"))
    return ParserSuccess(SIntType(TypeKind::Int32));
  if (parser.Read(TokenKind::Identifier, "int64"))
    return ParserSuccess(SIntType(TypeKind::Int64));

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<UIntType> ParseUInt(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "uint8"))
    return ParserSuccess(UIntType(TypeKind::UInt8));
  if (parser.Read(TokenKind::Identifier, "uint16"))
    return ParserSuccess(UIntType(TypeKind::UInt16));
  if (parser.Read(TokenKind::Identifier, "uint32"))
    return ParserSuccess(UIntType(TypeKind::UInt32));
  if (parser.Read(TokenKind::Identifier, "uint64"))
    return ParserSuccess(UIntType(TypeKind::UInt64));

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<FloatType> ParseFloat(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "float32"))
    return ParserSuccess(FloatType(TypeKind::Float32));
  if (parser.Read(TokenKind::Identifier, "float64"))
    return ParserSuccess(FloatType(TypeKind::Float64));

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<BoolType> ParseBool(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "bool"))
    return ParserSuccess(BoolType());

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<CharType> ParseChar(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "char"))
    return ParserSuccess(CharType());

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<StrType> ParseString(Parser &parser) {
  if (parser.Read(TokenKind::Identifier, "str"))
    return ParserSuccess(StrType());

  return ParserError(ParseErrorKind::UnexpectedToken, "");
}

ParseResult<SliceType> ParseSlice(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "["))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  if (!parser.Read(TokenKind::Symbol, "]"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  return ParserSuccess(SliceType(type.Value()));
}

ParseResult<ArrayType> ParseArray(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "["))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  if (!parser.Read(TokenKind::Symbol, ";"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto size = parser.ParseTerminal();

  if (!size)
    return size.Error();

  if (size.Kind() != TerminalKind::Integer)
    return ParserError(ParseErrorKind::ExpectedToken, "");

  if (!parser.Read(TokenKind::Symbol, "]"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  return ParserSuccess(ArrayType(type.Value(), Unwrap(size, Integer)));
}

ParseResult<RefType> ParseRef(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "&"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  return ParserSuccess(RefType(type.Value()));
}

ParseResult<PointerType> ParsePointer(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "*"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  return ParserSuccess(PointerType(type.Value()));
}

ParseResult<TypeNode> ParseType(Parser &parser) {
  auto sInt = ParseSInt(parser);
  if (sInt)
    return ParserSuccess(TypeNode(sInt.Value()));

  auto uInt = ParseUInt(parser);
  if (uInt)
    return ParserSuccess(TypeNode(uInt.Value()));

  auto floatType = ParseFloat(parser);
  if (floatType)
    return ParserSuccess(TypeNode(floatType.Value()));

  auto boolType = ParseBool(parser);
  if (boolType)
    return ParserSuccess(TypeNode(boolType.Value()));

  auto charType = ParseChar(parser);
  if (charType)
    return ParserSuccess(TypeNode(charType.Value()));

  auto strType = ParseString(parser);
  if (strType)
    return ParserSuccess(TypeNode(strType.Value()));

  auto sliceType = ParseSlice(parser);
  if (sliceType)
    return ParserSuccess(TypeNode(sliceType.Value()));

  auto arrayType = ParseArray(parser);
  if (arrayType)
    return ParserSuccess(TypeNode(arrayType.Value()));

  auto refType = ParseRef(parser);
  if (refType)
    return ParserSuccess(TypeNode(refType.Value()));

  auto pointerType = ParsePointer(parser);
  if (pointerType)
    return ParserSuccess(TypeNode(pointerType.Value()));

  return ParserError(ParseErrorKind::ExpectedType, "");
}

} // namespace aatbe::parser
