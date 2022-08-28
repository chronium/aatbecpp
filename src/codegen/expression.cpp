//
// Created by chronium on 28.08.2022.
//

#include <codegen.hpp>
#include <codegen/expression.hpp>

namespace aatbe::codegen {

std::optional<llvm::Value *> CodegenCall(CallExpression *call) {
  assert(call->Callee()->Kind() == ExpressionKind::Atom);
  assert(call->Callee()->AsAtom()->Value()->Kind() == TerminalKind::Identifier);
  assert(call->Args()->Kind() == ExpressionKind::Tuple);

  auto terminal = call->Callee()->AsAtom()->Value()->AsIdentifier();
  auto argsExprs = call->Args()->AsTuple()->Value();

  auto function = GetCompilerContext()->GetFunction(terminal->Value());

  std::vector<llvm::Value *> args;

  for (auto arg : argsExprs) {
    auto argValue = CodegenExpression(arg);
    if (!argValue) {
      return std::nullopt;
    }
    args.push_back(*argValue);
  }

  return GetLLVMBuilder()->CreateCall(function, args);
}

std::optional<llvm::Value *> CodegenExpression(ExpressionNode *expression) {
  switch (expression->Kind()) {
  case ExpressionKind::Atom:
    return CodegenAtom(expression->AsAtom());
  case ExpressionKind::Unary:
    assert(nullptr);
  case ExpressionKind::Binary:
    assert(nullptr);
  case ExpressionKind::Tuple:
    assert(nullptr);
  case ExpressionKind::Call:
    return CodegenCall(expression->AsCall());
  case ExpressionKind::Block:
    assert(nullptr);
  default:
    return {};
  }
}

std::optional<llvm::Value *> CodegenAtom(AtomExpression *atom) {
  auto term = atom->Value();
  switch (term->Kind()) {
  case TerminalKind::Identifier:
    assert(nullptr);
  case TerminalKind::Integer:
    assert(nullptr);
  case TerminalKind::String:
    return GetLLVMBuilder()->CreateGlobalStringPtr(term->AsString()->Value());
  default:
    return std::nullopt;
  }
}

} // namespace aatbe::codegen
