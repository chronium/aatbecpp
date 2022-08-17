#include <codegen.hpp>

#include <llvm/IR/LLVMContext.h>

namespace aatbe::codegen {

    static std::unique_ptr<llvm::LLVMContext> Context;

    std::unique_ptr<llvm::Module> compile_file(const std::string &file) {
        Context = std::make_unique<llvm::LLVMContext>();
        auto module = std::make_unique<llvm::Module>(file, *Context);

        return module;
    }

} // namespace aatbe::codegen
