#pragma once

#include <memory>

#include <concepts>
#include <lexer/lexer.hpp>
#include <optional>
#include <utility>

using namespace aatbe::lexer;

namespace aatbe::parser {

template <typename T>
concept AstNode = requires(T t) {
                    t.Value();
                    t.Kind();
                  };

enum class ParserErrorKind { InvalidToken };

// ParserError is a simple error class that holds the error kind and the
// error message.
class ParserError {
public:
  ParserError(ParserErrorKind kind, std::string message)
      : kind(kind), message(std::move(message)) {}

  ParserErrorKind Kind() const { return kind; }
  const std::string &Message() const { return message; }

private:
  ParserErrorKind kind;
  std::string message;
};

// ParserSuccess
template <typename Tin, typename Tout> struct ParserSuccess {
  ParserSuccess(std::vector<Tin> rest, Tout value) : value(value), rest(rest) {}
  Tout value;
  std::vector<Tin> rest;

  std::vector<Tin> Rest() const { return rest; }
  const Tout Value() const { return value; }
};

// ParseResult type is either a ParserError or a ParserSuccess
template <typename Tin, AstNode Tout> struct ParseResult {
  using SuccessType = ParserSuccess<Tin, Tout>;
  using ErrorType = ParserError;
  using Type = std::variant<SuccessType, ErrorType>;
  Type value;
  ParseResult(SuccessType success) : value(success) {}
  ParseResult(ErrorType error) : value(error) {}
  explicit operator bool() const {
    return std::holds_alternative<SuccessType>(value);
  }
  // Kind
  auto Kind() { return std::get<SuccessType>(value).Value().Kind(); }
  auto Value() { return std::get<SuccessType>(value).Value().Value(); }
  std::vector<Tin> Rest() { return std::get<SuccessType>(value).Rest(); }
  ErrorType &Error() { return std::get<ErrorType>(value); }
  const SuccessType &Value() const { return std::get<SuccessType>(value); }
  const ErrorType &Error() const { return std::get<ErrorType>(value); }
};

// Parser is a function that takes a vector of tokens and returns a ParseResult
template <typename Tin, AstNode Tout>
using Parser = std::function<ParseResult<Tin, Tout>(std::vector<Tin>)>;

} // namespace aatbe::parser