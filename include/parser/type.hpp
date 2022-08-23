//
// Created by chronium on 22.08.2022.
//

#pragma once

#include <parser/parser.hpp>
#include <string>
#include <vector>

namespace aatbe::parser {

enum TypeKind {
  Int8,
  Int16,
  Int32,
  Int64,
  UInt8,
  UInt16,
  UInt32,
  UInt64,
  Float32,
  Float64,
  Bool,
  Char,
  Str,
  Array,
  Ref,
  Pointer,
  Slice,
};

struct Type {
  Type() = delete;

  explicit Type(TypeKind kind) : kind(kind) {}

  auto Kind() const { return this->kind; }

  auto Value() const { return this->Kind(); }

private:
  TypeKind kind;
};

struct SIntType : Type {
  explicit SIntType(TypeKind kind) : Type(kind) {
    assert(kind == TypeKind::Int8 || kind == TypeKind::Int16 ||
           kind == TypeKind::Int32 || kind == TypeKind::Int64);
  }
};

struct UIntType : Type {
  explicit UIntType(TypeKind kind) : Type(kind) {
    assert(kind == TypeKind::UInt8 || kind == TypeKind::UInt16 ||
           kind == TypeKind::UInt32 || kind == TypeKind::UInt64);
  }
};

struct FloatType : Type {
  explicit FloatType(TypeKind kind) : Type(kind) {
    assert(kind == TypeKind::Float32 || kind == TypeKind::Float64);
  }
};

struct BoolType : Type {
  explicit BoolType() : Type(TypeKind::Bool) {}
};

struct CharType : Type {
  explicit CharType() : Type(TypeKind::Char) {}
};

struct StrType : Type {
  explicit StrType() : Type(TypeKind::Str) {}
};

struct TypeNode;

struct SliceType : Type {
  explicit SliceType(std::shared_ptr<Type> type)
      : Type(TypeKind::Slice), type(std::move(type)) {}

  auto Inner() const { return this->type; }

private:
  std::shared_ptr<Type> type;
};

struct ArrayType : Type {
  explicit ArrayType(std::shared_ptr<Type> type, uint64_t size)
      : Type(TypeKind::Array), type(std::move(type)), size(size) {}

  auto Inner() const { return this->type; }
  auto Size() const { return this->size; }

private:
  std::shared_ptr<Type> type;
  uint64_t size;
};

struct RefType : Type {
  explicit RefType(std::shared_ptr<Type> type)
      : Type(TypeKind::Ref), type(std::move(type)) {}

  auto Inner() const { return this->type; }

private:
  std::shared_ptr<Type> type;
};

struct PointerType : Type {
  explicit PointerType(std::shared_ptr<Type> type)
      : Type(TypeKind::Pointer), type(std::move(type)) {}

  auto Inner() const { return this->type; }

private:
  std::shared_ptr<Type> type;
};

struct TypeNode {
  TypeNode() = delete;

  template <typename T>
  explicit TypeNode(T value): value(std::make_shared<Type>(value)) {}

  TypeKind Kind() const { return value->Kind(); }
  std::shared_ptr<Type> Value() const { return value; }

  auto AsSInt() const -> std::shared_ptr<SIntType> {
    return std::static_pointer_cast<SIntType>(value);
  }

  auto AsUInt() const -> std::shared_ptr<UIntType> {
    return std::static_pointer_cast<UIntType>(value);
  }

  auto AsFloat() const -> std::shared_ptr<FloatType> {
    return std::static_pointer_cast<FloatType>(value);
  }

  auto AsBool() const -> std::shared_ptr<BoolType> {
    return std::static_pointer_cast<BoolType>(value);
  }

  auto AsChar() const -> std::shared_ptr<CharType> {
    return std::static_pointer_cast<CharType>(value);
  }

  auto AsStr() const -> std::shared_ptr<StrType> {
    return std::static_pointer_cast<StrType>(value);
  }

  auto AsSlice() const -> std::shared_ptr<SliceType> {
    return std::static_pointer_cast<SliceType>(value);
  }

  auto AsArray() const -> std::shared_ptr<ArrayType> {
    return std::static_pointer_cast<ArrayType>(value);
  }

  auto AsRef() const -> std::shared_ptr<RefType> {
    return std::static_pointer_cast<RefType>(value);
  }

  auto AsPointer() const -> std::shared_ptr<PointerType> {
    return std::static_pointer_cast<PointerType>(value);
  }

private:
  std::shared_ptr<Type> value;
};

ParseResult<TypeNode> ParseType(Parser &parser);

} // namespace aatbe::parser

