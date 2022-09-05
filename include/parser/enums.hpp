//
// Created by chronium on 26.08.2022.
//

#pragma once

namespace aatbe::parser {

enum class ModuleStatementKind {
  Import,
  Export,
  Function,
  Global,
  Constant,
  Struct,
  Typename,
  Type,
  ParameterBinding,
  ParameterList,
  Module,
  MemberBinding,
  MemberList,
};

enum class VisibilityModifier {
  Public,
  Internal,
};

} // namespace aatbe::parser
