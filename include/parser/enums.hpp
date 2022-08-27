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
  Structure,
  Typename,
  Type,
  ParameterBinding,
  ParameterList,
  Module,
};

} // namespace aatbe::parser
