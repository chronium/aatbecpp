//
// Created by chronium on 26.08.2022.
//

#pragma once

#include <parser/type.hpp>
#include <parser/enums.hpp>

#include <string>

namespace aatbe::parser {

struct ParameterBinding {
  ParameterBinding() = delete;
  ParameterBinding(ParameterBinding &&) = delete;
  ParameterBinding(ParameterBinding const &) = delete;
  ParameterBinding &operator=(ParameterBinding const &) = delete;

  ParameterBinding(TypeNode *type, std::string name)
      : name(std::move(name)), type(type) {}

  auto Name() const { return name; }
  auto Type() const { return type; }

  std::string Format() const { return name + " : " + type->Format(); }

  auto Value() const { return std::make_tuple(name, type); }
  auto Kind() const { return ModuleStatementKind::ParameterBinding; }

private:
  std::string name;
  TypeNode *type;
};

struct ParameterList {
  ParameterList() = delete;
  ParameterList(ParameterList &&) = delete;
  ParameterList(ParameterList const &) = delete;
  ParameterList &operator=(ParameterList const &) = delete;

  explicit ParameterList(std::vector<ParameterBinding *> bindings)
      : bindings(std::move(bindings)) {}

  std::string Format() const {
    std::string result = "(";
    for (auto binding : bindings) {
      result += binding->Format();
      result += ", ";
    }
    result += ")";
    return result;
  }

  auto Bindings() const { return bindings; }
  auto Size() const { return bindings.size(); }

  auto Value() const { return bindings; }
  auto Kind() const { return ModuleStatementKind::ParameterList; }

private:
  std::vector<ParameterBinding *> bindings;
};

struct MemberBinding {
  MemberBinding() = delete;
  MemberBinding(MemberBinding &&) = delete;
  MemberBinding(MemberBinding const &) = delete;
  MemberBinding &operator=(MemberBinding const &) = delete;

  MemberBinding(TypeNode *type, std::string name, VisibilityModifier visibility)
      : name(std::move(name)), type(type), visibility(visibility) {}

  auto Name() const { return name; }
  auto Type() const { return type; }
  auto Visibility() const { return visibility; }

  std::string Format() const { return name + " : " + type->Format(); }

  auto Value() const { return std::make_tuple(name, type); }
  auto Kind() const { return ModuleStatementKind::MemberBinding; }

private:
  std::string name;
  TypeNode *type;
  VisibilityModifier visibility;
};

struct MemberList {
  MemberList() = delete;
  MemberList(MemberList &&) = delete;
  MemberList(MemberList const &) = delete;
  MemberList &operator=(MemberList const &) = delete;

  explicit MemberList(std::vector<MemberBinding *> bindings)
      : bindings(std::move(bindings)) {}

  std::string Format() const {
    std::string result = "(";
    for (auto binding : bindings) {
      result += binding->Format();
      result += ", ";
    }
    result += ")";
    return result;
  }

  auto Bindings() const { return bindings; }
  auto Size() const { return bindings.size(); }

  auto Value() const { return bindings; }
  auto Kind() const { return ModuleStatementKind::MemberList; }

private:
  std::vector<MemberBinding *> bindings;
};

} // namespace aatbe::parser