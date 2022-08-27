#include <codegen.hpp>

#include <parser/parser.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

using namespace aatbe::source;
using namespace aatbe::lexer;
using namespace aatbe::parser;

namespace aatbe::codegen {

static std::shared_ptr<llvm::LLVMContext> Context;
static std::shared_ptr<llvm::IRBuilder<>> Builder;
static std::shared_ptr<llvm::Module> Module;

std::shared_ptr<llvm::LLVMContext> GetLLVMContext() { return Context; }
std::shared_ptr<llvm::IRBuilder<>> GetLLVMBuilder() { return Builder; }
std::shared_ptr<llvm::Module> GetLLVMModule() { return Module; }

auto DeclPass(ModuleNode *mod) {
  for (auto &statement : mod->Value()) {
    switch (statement->Kind()) {
    case ModuleStatementKind::Function: {
      auto funcDecl = statement->AsFunction();
      auto funcType = funcDecl->Type();
      auto funcName = funcDecl->Name();
      auto funcParams = funcDecl->Parameters();

      std::vector<llvm::Type *> paramTypes;
      for (auto &param : funcParams->Bindings()) {
        paramTypes.push_back(param->Type()->LLVMType());
      }

      llvm::Function::Create(
          llvm::FunctionType::get(funcType, paramTypes, false),
          llvm::Function::ExternalLinkage, funcName, Module.get());

      break;
    }
    default:
      break;
    }
  }
}

std::shared_ptr<llvm::Module> compile_file(const std::string &file) {
  Context = std::make_shared<llvm::LLVMContext>();
  Module = std::make_shared<llvm::Module>(file, *Context);
  Builder = std::make_shared<llvm::IRBuilder<>>(*Context);

  auto srcFile = SrcFile::FromFile(file);

  Lexer lexer(std::move(srcFile));
  auto tokens = lexer.Lex();

  Parser parser(tokens);

  auto mod = parser.Parse();

  DeclPass(mod.Node());

  return std::move(Module);
}

} // namespace aatbe::codegen
