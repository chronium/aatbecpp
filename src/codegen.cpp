#include <codegen.hpp>
#include <codegen/context.hpp>
#include <codegen/expression.hpp>

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
static std::shared_ptr<CompilerContext> CompContext;

std::shared_ptr<llvm::LLVMContext> GetLLVMContext() { return Context; }
std::shared_ptr<llvm::IRBuilder<>> GetLLVMBuilder() { return Builder; }
std::shared_ptr<llvm::Module> GetLLVMModule() { return Module; }
std::shared_ptr<CompilerContext> GetCompilerContext() { return CompContext; }

auto DeclPass(ModuleNode *mod) {
  for (auto &statement : mod->Value()) {
    switch (statement->Kind()) {
    case ModuleStatementKind::Function: {
      auto funcDecl = statement->AsFunction();
      auto funcType = funcDecl->Type();
      auto funcName = funcDecl->Name();
      auto funcParams = funcDecl->Parameters();
      auto funcVariadic = funcDecl->IsVariadic();

      std::vector<llvm::Type *> paramTypes;
      for (auto &param : funcParams->Bindings()) {
        paramTypes.push_back(param->Type()->LLVMType());
      }

      GetCompilerContext()->CurrentScope()->SetFunction(
          funcName,
          llvm::Function::Create(
              llvm::FunctionType::get(funcType, paramTypes, funcVariadic),
              llvm::Function::ExternalLinkage, funcName, Module.get()));

      break;
    }
    default:
      break;
    }
  }
}

auto CodegenPass(ModuleNode *mod) {
  for (auto &statement : mod->Value()) {
    switch (statement->Kind()) {
    case ModuleStatementKind::Function: {
      auto funcDecl = statement->AsFunction();
      auto funcName = funcDecl->Name();

      if (funcDecl->IsExtern() || !funcDecl->Body().has_value()) {
        continue;
      }

      auto func = Module->getFunction(funcName);

      auto bb = funcName == "main"
                    ? llvm::BasicBlock::Create(*Context, "entry", func)
                    : llvm::BasicBlock::Create(*Context, "", func);

      Builder->SetInsertPoint(bb);

      GetCompilerContext()->EnterScope(funcName);

      std::optional<llvm::Value *> retVal = CodegenExpression(*funcDecl->Body());

      if (funcDecl->ReturnType()->Kind() == TypeKind::Unit)
        Builder->CreateRetVoid();
      else
        Builder->CreateRet(*retVal);

      GetCompilerContext()->ExitScope();

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
  CompContext = std::make_shared<CompilerContext>();

  auto srcFile = SrcFile::FromFile(file);

  Lexer lexer(std::move(srcFile));
  auto tokens = lexer.Lex();

  Parser parser(tokens);

  auto mod = parser.Parse();

  CompContext->EnterScope("root");
  DeclPass(mod.Node());
  CodegenPass(mod.Node());
  CompContext->ExitScope();

  return std::move(Module);
}

} // namespace aatbe::codegen
