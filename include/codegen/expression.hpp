//
// Created by chronium on 28.08.2022.
//

#pragma once

#include <llvm/IR/Value.h>

#include <optional>

#include <parser/expression.hpp>

using namespace aatbe::parser;

namespace aatbe::codegen {

std::optional<llvm::Value *> CodegenExpression(ExpressionNode *expression);
std::optional<llvm::Value *> CodegenCall(CallExpression *call);
std::optional<llvm::Value *> CodegenAtom(AtomExpression *atom);
std::optional<llvm::Value *> CodegenIf(IfExpression *ifExpr);

} // namespace aatbe::codegen
