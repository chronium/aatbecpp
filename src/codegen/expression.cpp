//
// Created by chronium on 28.08.2022.
//

#include <codegen.hpp>
#include <codegen/expression.hpp>

#include <parser/type.hpp>

#include <llvm/IR/Constants.h>

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
  std::optional<llvm::Value *> result = {};

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
    for (auto expr : expression->AsBlock()->Value()) {
      result = CodegenExpression(expr);
      if (!result) {
        return std::nullopt;
      }
    }
    return result;
  default:
    return {};
  }
}

llvm::Value *ConstantInteger(IntegerTerm *term) {
  switch (term->Type()->Kind()) {
  case TypeKind::Int8:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt8Ty(), term->Value(),
                                  true);
  case TypeKind::Int16:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt16Ty(), term->Value(),
                                  true);
  case TypeKind::Int32:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt32Ty(), term->Value(),
                                  true);
  case TypeKind::Int64:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt64Ty(), term->Value(),
                                  true);

  case TypeKind::UInt8:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt8Ty(), term->Value(),
                                  false);
  case TypeKind::UInt16:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt16Ty(), term->Value(),
                                  false);
  case TypeKind::UInt32:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt32Ty(), term->Value(),
                                  false);
  case TypeKind::UInt64:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt64Ty(), term->Value(),
                                  false);
  default:
    assert(nullptr);
    return nullptr;
  }
}

std::optional<llvm::Value *> CodegenAtom(AtomExpression *atom) {
  auto term = atom->Value();
  switch (term->Kind()) {
  case TerminalKind::Identifier:
    assert(nullptr);
  case TerminalKind::Integer:
    return ConstantInteger(term->AsInteger());
  case TerminalKind::String:
    return GetLLVMBuilder()->CreateGlobalStringPtr(term->AsString()->Value());
  default:
    return std::nullopt;
  }
}

} // namespace aatbe::codegen
