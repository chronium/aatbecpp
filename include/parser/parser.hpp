#pragma once

#include <lexer/lexer.hpp>
#include <parser/ast.hpp>

#include <memory>
#include <concepts>
#include <optional>
#include <utility>
#include <vector>
#include <variant>

using namespace aatbe::lexer;

namespace aatbe::parser {

#define Indirect(NODE, Type) NODE->Value()->As##Type()->Value()
#define Unwrap(NODE, Type) NODE.Node()->As##Type()->Value()
#define Dig(NODE, Type, What) NODE.Node()->As##Type()->What()

#define TryReturn(Expr) \
  if (auto Result = (Expr)) { \
    return Result; \
  }

#define TryReturnOrError(Expr) \
  if (auto Result = (Expr)) { \
    return Result; \
  } else { \
    return Result.Error(); \
  }

#define ErrorOrContinue(Name, Expr) \
  auto(Name) = (Expr);               \
  if (!(Name))                        \
    return (Name).Error();

#define Deffer(expr) [](Parser &parser) { return (expr); }

template <typename T>
concept AstNode = requires(T t) {
                    t->Value();
                    t->Kind();
                    { t->Format() } -> std::convertible_to<std::string>;
                  };

enum class ParseErrorKind {
  InvalidToken,
  UnexpectedToken,
  UnexpectedEof,
  ExpectedToken,
  ExpectedType,
  ExpectedTerminal,
  ExpectedSymbol,
  InvalidOperator,
  ExpectedExpression,
};

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
template <AstNode Tout> class ParseResult {
public:
  using SuccessType = ParserSuccess<Tout>;
  using ErrorType = ParserError;
  using Type = std::variant<SuccessType, ErrorType>;

  ParseResult(SuccessType success) : value(success) {}
  ParseResult(ErrorType error) : value(error) {}
  explicit operator bool() const {
    return std::holds_alternative<SuccessType>(value);
  }

  auto Node() { return std::get<SuccessType>(value).Value(); }
  auto Kind() { return std::get<SuccessType>(value).Value()->Kind(); }
  auto Value() { return std::get<SuccessType>(value).Value()->Value(); }
  auto Format() { return std::get<SuccessType>(value).Value()->Format(); }
  ErrorType &Error() { return std::get<ErrorType>(value); }
  const SuccessType &Value() const { return std::get<SuccessType>(value); }
  const ErrorType &Error() const { return std::get<ErrorType>(value); }

  template <typename T, typename... Args>
  ParseResult<T *> WrapWith(Args... args) {
    if (*this)
      return ParserSuccess(new T(this->Node(), args...));
    else
      return this->Error();
  }
private:
  Type value;
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

  explicit Parser(std::vector<Token *> tokens) : tokens(std::move(tokens)) {}
  Parser(Parser &&) = default;
  Parser(const Parser &) = delete;
  Parser &operator=(Parser &&) = delete;
  Parser &operator=(const Parser &) = delete;
  ~Parser() = default;

  auto PeekKeyword(const char *keyword);
  auto ReadKeyword(const char *keyword);

  bool Match(TokenKind kind);

  auto Peek(off_t offset = 0) {
    return index + offset < this->tokens.size()
               ? std::optional<std::reference_wrapper<
                     Token *>>{this->tokens[index + offset]}
               : std::nullopt;
  }
  auto Peek(TokenKind kind, const char *valueS) {
    if (auto token = this->Peek())
      return token->get()->Kind() == kind && token->get()->ValueS() == valueS;

    return false;
  }
  auto Peek(const char *valueS, off_t offset = 0) {
    if (auto token = this->Peek(offset))
      return token->get()->ValueS() == valueS;

    return false;
  }

  auto Read() {
    return index < this->tokens.size()
               ? std::optional<
                     std::reference_wrapper<Token *>>{this->tokens[index++]}
               : std::nullopt;
  }
  auto Read(TokenKind kind, const char *valueS) {
    return this->Peek(kind, valueS) ? this->Read() : std::nullopt;
  }
  auto Read(const char *valueS) {
    return this->Peek(valueS) ? this->Read() : std::nullopt;
  }

  ParseResult<ModuleNode *> Parse();

  Memo Snapshot() const { return Memo{index}; }
  void Restore(Memo memo) { index = memo.Index(); }

  template <typename F> auto Try(F f) {
    auto memo = Snapshot();
    auto result = f(*this);

    if (!result)
      Restore(memo);

    return result;
  }

  template <typename F>
  auto DelimitedBy(F f, TokenKind kind, const char *delimiter) {
    std::vector<decltype(f(*this))> results;

    while (true) {
      auto result = f(*this);

      if (!result)
        break;

      results.push_back(result);

      if (!Read(kind, delimiter))
        break;
    }

    return results;
  }

  template <typename F>
  auto SurroundedBy(F parser, TokenKind kind, const char *open, TokenKind kind2,
                    const char *close) {
    if (!Read(kind, open))
      return ParseResult<decltype(parser(*this))>(
          ParserError(ParseErrorKind::ExpectedToken, ""));

    auto result = parser(*this);

    if (!Read(kind2, close))
      return ParseResult<decltype(parser(*this))>(
          ParserError(ParseErrorKind::ExpectedToken, ""));

    return ParseResult(ParserSuccess(result));
  }

private:
  std::vector<Token *> tokens;
  size_t index = 0;
};

ParseResult<FunctionStatement *> ParseFunction(Parser &parser);
ParseResult<ModuleStatementNode *> ParseModuleStatement(Parser &parser);
ParseResult<TypeNode *> ParseType(Parser &parser);
ParseResult<TerminalNode *> ParseTerminal(Parser &parser);
ParseResult<ExpressionNode *> ParseExpression(Parser &tokens);
ParseResult<ParameterBinding *> ParseParameter(Parser &parser);

} // namespace aatbe::parser