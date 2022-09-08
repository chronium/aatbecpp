//
// Created by chronium on 28.08.2022.
//

#pragma once

#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

#include <llvm/IR/Function.h>

namespace aatbe::codegen {

class Scope {
public:
  Scope() = default;
  Scope(Scope &&) = delete;
  Scope(Scope const &) = delete;
  Scope &operator=(Scope const &) = delete;

  explicit Scope(std::string scopeName) : scopeName(std::move(scopeName)) {}

  auto GetFunction(const std::string &name) { return functions[name]; }
  auto SetFunction(const std::string &name, llvm::Function *function) {
    this->functions[name] = function;
  }

  auto GetStruct(const std::string &name) { return structs[name]; }
  auto SetStruct(const std::string &name, llvm::StructType *structType) {
    this->structs[name] = structType;
  }

  auto ScopeName() const { return scopeName; }

private:
  std::string scopeName{};
  std::unordered_map<std::string, llvm::Function *> functions{};
  std::unordered_map<std::string, llvm::StructType *> structs{};
};

class TypeSystem;

class CompilerContext {
public:
  CompilerContext() = default;
  CompilerContext(CompilerContext &&) = delete;
  CompilerContext(CompilerContext const &) = delete;
  CompilerContext &operator=(CompilerContext const &) = delete;

  auto EnterScope(std::string scope = {}) {
    scopes.push_back(std::make_shared<Scope>(std::move(scope)));
  }

  auto ExitScope() { scopes.pop_back(); }

  auto CurrentScope() { return scopes[scopes.size() - 1]; }
  auto CurrentScopeName() { return scopes[scopes.size() - 1]->ScopeName(); }

  auto GetFunction(const std::string &name) {
    llvm::Function *result = nullptr;
    for (unsigned i = scopes.size(); i-- > 0;) {
      auto scope = scopes[i];
      if (auto function = scope->GetFunction(name)) {
        result = function;
        break;
      }
    }

    return result;
  }

  auto GetStruct(const std::string &name) {
    llvm::StructType *result = nullptr;
    for (unsigned i = scopes.size(); i-- > 0;) {
      auto scope = scopes[i];
      if (auto structType = scope->GetStruct(name)) {
        result = structType;
        break;
      }
    }

    return result;
  }

private:
  std::vector<std::shared_ptr<Scope>> scopes{};
};

} // namespace aatbe::codegen
