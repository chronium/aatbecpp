#pragma once

#include <llvm/IR/Module.h>

namespace aatbe::codegen {
std::unique_ptr<llvm::Module> compile_file(const std::string &file);
} // namespace aatbe::codegen
