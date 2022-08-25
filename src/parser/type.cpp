//
// Created by chronium on 22.08.2022.
//

#include <parser/parser.hpp>
#include <parser/terminal.hpp>
#include <parser/type.hpp>

#include <vector>
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

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  if (!parser.Read(TokenKind::Symbol, "]"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  return ParserSuccess(new SliceType(*type.Node()));
}

ParseResult<ArrayType *> ParseArray(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "["))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  if (!parser.Read(TokenKind::Symbol, ";"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto size = ParseTerminal(parser);

  if (!size)
    return size.Error();

  if (size.Kind() != TerminalKind::Integer)
    return ParserError(ParseErrorKind::ExpectedToken, "");

  if (!parser.Read(TokenKind::Symbol, "]"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  return ParserSuccess(new ArrayType(*type.Node(), Unwrap(size, Integer)));
}

ParseResult<RefType *> ParseRef(Parser &parser) {
  if (!parser.Read(TokenKind::Keyword, "ref"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  return ParserSuccess(new RefType(*type.Node()));
}

ParseResult<PointerType *> ParsePointer(Parser &parser) {
  if (!parser.Read(TokenKind::Symbol, "*"))
    return ParserError(ParseErrorKind::ExpectedToken, "");

  auto type = ParseType(parser);

  if (!type)
    return type.Error();

  return ParserSuccess(new PointerType(*type.Node()));
}

ParseResult<TypeNode *> ParseType(Parser &parser) {
  auto sInt = parser.Try(ParseSInt);
  if (sInt)
    return ParserSuccess(new TypeNode(sInt.Node()));

  auto uInt = parser.Try(ParseUInt);
  if (uInt)
    return ParserSuccess(new TypeNode(uInt.Node()));

  auto floatType = parser.Try(ParseFloat);
  if (floatType)
    return ParserSuccess(new TypeNode(floatType.Node()));

  auto boolType = parser.Try(ParseBool);
  if (boolType)
    return ParserSuccess(new TypeNode(boolType.Node()));

  auto charType = parser.Try(ParseChar);
  if (charType)
    return ParserSuccess(new TypeNode(charType.Node()));

  auto strType = parser.Try(ParseString);
  if (strType)
    return ParserSuccess(new TypeNode(strType.Node()));

  auto sliceType = parser.Try(ParseSlice);
  if (sliceType)
    return ParserSuccess(new TypeNode(sliceType.Node()));

  auto arrayType = parser.Try(ParseArray);
  if (arrayType)
    return ParserSuccess(new TypeNode(arrayType.Node()));

  auto refType = parser.Try(ParseRef);
  if (refType)
    return ParserSuccess(new TypeNode(refType.Node()));

  auto pointerType = parser.Try(ParsePointer);
  if (pointerType)
    return ParserSuccess(new TypeNode(pointerType.Node()));

  return ParserError(ParseErrorKind::ExpectedType, "");
}

} // namespace aatbe::parser
