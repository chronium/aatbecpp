#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <codegen/context.hpp>

namespace aatbe::codegen {

extern std::unique_ptr<llvm::LLVMContext> LLVMContext;
extern std::shared_ptr<llvm::IRBuilder<>> Builder;
extern std::unique_ptr<llvm::Module> Module;
extern std::shared_ptr<CompilerContext> CompContext;

std::shared_ptr<llvm::IRBuilder<>> GetLLVMBuilder();
std::shared_ptr<CompilerContext> GetCompilerContext();

void compile_file(const std::string &file);
} // namespace aatbe::codegen
