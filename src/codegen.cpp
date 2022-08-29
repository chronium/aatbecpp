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

std::unique_ptr<llvm::LLVMContext> LLVMContext;
std::shared_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> Module;
std::shared_ptr<CompilerContext> CompContext;

std::shared_ptr<llvm::IRBuilder<>> GetLLVMBuilder() { return Builder; }
std::shared_ptr<CompilerContext> GetCompilerContext() { return CompContext; }

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

      GetCompilerContext()->CurrentScope()->SetFunction(
          funcName,
          llvm::Function::Create(funcType,
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
                    ? llvm::BasicBlock::Create(*LLVMContext, "entry", func)
                    : llvm::BasicBlock::Create(*LLVMContext, "", func);

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

void compile_file(const std::string &file) {
  LLVMContext = std::make_unique<llvm::LLVMContext>();
  Module = std::make_unique<llvm::Module>(file, *LLVMContext);
  Builder = std::make_shared<llvm::IRBuilder<>>(*LLVMContext);
  CompContext = std::make_shared<CompilerContext>();

  auto srcFile = SrcFile::FromFile(file);

  Lexer lexer(std::move(srcFile));
  auto tokens = lexer.Lex();

  Parser parser(tokens);

  auto mod = parser.Parse();
  printf("%s\n", mod.Format().c_str());

  CompContext->EnterScope("root");
  DeclPass(mod.Node());
  CodegenPass(mod.Node());
  CompContext->ExitScope();
}

} // namespace aatbe::codegen
