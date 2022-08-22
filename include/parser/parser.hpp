#pragma once

#include <memory>

#include "terminals.hpp"
#include <concepts>
#include <lexer/lexer.hpp>
#include <optional>
#include <utility>

using namespace aatbe::lexer;

namespace aatbe::parser {

#define Unwrap(NODE, Type) NODE.Node().As##Type()->Value()

template <typename T>
concept AstNode = requires(T t) {
                    t.Value();
                    t.Kind();
                  };

enum class ParseErrorKind { InvalidToken, UnexpectedToken, UnexpectedEof };

// ParserError is a simple error class that holds the error kind and the
// error message.
class ParserError {
public:
  ParserError(ParseErrorKind kind, std::string message)
      : kind(kind), message(std::move(message)) {}

  ParseErrorKind Kind() const { return kind; }
  const std::string &Message() const { return message; }

private:
  ParseErrorKind kind;
  std::string message;
};

// ParserSuccess
template <typename Tout> struct ParserSuccess {
  ParserSuccess(Tout value) : value(value) {}
  Tout value;

  const Tout Value() const { return value; }
};

// ParseResult type is either a ParserError or a ParserSuccess
template <AstNode Tout> struct ParseResult {
  using SuccessType = ParserSuccess<Tout>;
  using ErrorType = ParserError;
  using Type = std::variant<SuccessType, ErrorType>;
  Type value;
  ParseResult(SuccessType success) : value(success) {}
  ParseResult(ErrorType error) : value(error) {}
  explicit operator bool() const {
    return std::holds_alternative<SuccessType>(value);
  }
  // Kind
  auto Node() { return std::get<SuccessType>(value).Value(); }
  auto Kind() { return std::get<SuccessType>(value).Value().Kind(); }
  auto Value() { return std::get<SuccessType>(value).Value().Value(); }
  ErrorType &Error() { return std::get<ErrorType>(value); }
  const SuccessType &Value() const { return std::get<SuccessType>(value); }
  const ErrorType &Error() const { return std::get<ErrorType>(value); }
};

class Parser {
public:
  struct Memo {
    Memo() = delete;
    Memo(uint64_t index) : index(index) {}

    auto Index() const { return index; }

  private:
    uint64_t index;
  };

  Parser(std::vector<Token *> tokens) : tokens(std::move(tokens)) {}
  Parser(Parser &&) = default;
  Parser(const Parser &) = delete;
  Parser &operator=(Parser &&) = delete;
  Parser &operator=(const Parser &) = delete;
  ~Parser() = default;

  auto PeekKeyword(const char *keyword);
  auto ReadKeyword(const char *keyword);

  auto Peek(off_t offset = 0) {
    return index + offset < this->tokens.size()
               ? std::optional<std::reference_wrapper<
                     Token *>>{this->tokens[index + offset]}
               : std::nullopt;
  }

  auto Read() {
    return index < this->tokens.size()
               ? std::optional<
                     std::reference_wrapper<Token *>>{this->tokens[index++]}
               : std::nullopt;
  }

  ParseResult<TerminalNode> ParseTerminal();
  ParseResult<ModuleStatement> ParseModuleStatement();

  std::unique_ptr<ModuleNode> Parse();

  Memo Snapshot() const { return Memo{index}; }
  void Restore(Memo memo) { index = memo.Index(); }

  template <typename F> auto Fallible(F f) {
    auto memo = Snapshot();
    auto result = f();

    if (!result)
      Restore(memo);

    return result;
  }

private:
  std::vector<Token *> tokens;
  size_t index = 0;
};

} // namespace aatbe::parser