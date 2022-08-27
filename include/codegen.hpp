#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace aatbe::codegen {

std::shared_ptr<llvm::LLVMContext> GetLLVMContext();
std::shared_ptr<llvm::IRBuilder<>> GetLLVMBuilder();
std::shared_ptr<llvm::Module> GetLLVMModule();

std::shared_ptr<llvm::Module> compile_file(const std::string &file);
} // namespace aatbe::codegen
