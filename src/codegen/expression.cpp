//
// Created by chronium on 28.08.2022.
//

#include <codegen.hpp>
#include <codegen/expression.hpp>

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
  case ExpressionKind::If:
    return CodegenIf(expression->AsIf());
  case ExpressionKind::Loop:
    assert(nullptr);
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
  case TerminalKind::Boolean:
    return llvm::ConstantInt::get(GetLLVMBuilder()->getInt1Ty(),
                                  term->AsBoolean()->Value());
  default:
    return std::nullopt;
  }
}

std::optional<llvm::Value *> CodegenIf(IfExpression *ifExpr) {
  auto hasElse =
      std::get<0>(ifExpr->Value()[ifExpr->Value().size() - 1]) == nullptr;
  llvm::Type *bodyType = nullptr;

  auto function = GetLLVMBuilder()->GetInsertBlock()->getParent();
  auto &entry = function->getEntryBlock();

  std::vector<llvm::BasicBlock *> branchBlocks;
  std::vector<llvm::Value *> branchValues;

  auto mergeBlock = llvm::BasicBlock::Create(*LLVMContext, "merge");

  // Create target blocks
  for (size_t _i = 0; _i < ifExpr->Size(); _i++) {
    auto thenBlock = llvm::BasicBlock::Create(*LLVMContext, "branch", function);
    branchBlocks.push_back(thenBlock);
  }

  // Create jump list
  for (size_t i = 0; i < ifExpr->Size() - hasElse; i++) {
    auto expr = std::get<0>(ifExpr->Value()[i]);
    auto condition = CodegenExpression(expr);
    if (!condition) {
      return std::nullopt;
    }

    auto thenBlock = branchBlocks[i];
    auto bailBlock =
        hasElse ? branchBlocks[ifExpr->Value().size() - 1] : mergeBlock;

    GetLLVMBuilder()->CreateCondBr(
        GetLLVMBuilder()->CreateICmpEQ(
            condition.value(),
            llvm::ConstantInt::get(GetLLVMBuilder()->getInt1Ty(), 1, true)),
        thenBlock, bailBlock);
  }

  // FillBlocks
  for (size_t i = 0; i < ifExpr->Size(); i++) {
    auto expr = std::get<1>(ifExpr->Value()[i]);
    auto thenBlock = branchBlocks[i];

    GetLLVMBuilder()->SetInsertPoint(thenBlock);
    auto result = CodegenExpression(expr);
    if (!result) {
      return std::nullopt;
    }

    if (bodyType == nullptr && result.has_value())
      bodyType = result.value()->getType();
    else
      assert(bodyType == result.value()->getType());

    branchValues.push_back(result.value());

    GetLLVMBuilder()->CreateBr(mergeBlock);
  }

  function->getBasicBlockList().push_back(mergeBlock);
  GetLLVMBuilder()->SetInsertPoint(mergeBlock);

  if (bodyType == nullptr)
    return llvm::ConstantDataSequential::getNullValue(
        GetLLVMBuilder()->getVoidTy());

  auto phi = GetLLVMBuilder()->CreatePHI(bodyType, ifExpr->Size());

  for (size_t i = 0; i < ifExpr->Size(); i++) {
    phi->addIncoming(branchValues[i], branchBlocks[i]);
  }

  if (!hasElse)
    phi->addIncoming(llvm::ConstantDataSequential::getNullValue(bodyType),
                     &entry);

  return phi;
}

} // namespace aatbe::codegen
