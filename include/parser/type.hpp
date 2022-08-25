//
// Created by chronium on 22.08.2022.
//

#pragma once

#include <parser/parser.hpp>

#include <string>
#include <vector>
#include <cassert>

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

struct TypeNode;

struct Type {
  Type() = default;
  Type(Type &&) = delete;
  Type(Type const &) = delete;
  Type &operator=(Type const &) = delete;

  virtual ~Type() = default;

  virtual TypeKind Kind() const = 0;
  virtual TypeKind Value() const { return Kind(); }
};

struct SIntType : public Type {
  explicit SIntType(TypeKind kind) : kind(kind) {
    assert(kind == TypeKind::Int8 || kind == TypeKind::Int16 ||
           kind == TypeKind::Int32 || kind == TypeKind::Int64);
  }

  TypeKind Kind() const override { return this->kind; }

private:
  TypeKind kind;
};

struct UIntType : public Type {
  explicit UIntType(TypeKind kind) : kind(kind) {
    assert(kind == TypeKind::UInt8 || kind == TypeKind::UInt16 ||
           kind == TypeKind::UInt32 || kind == TypeKind::UInt64);
  }

  TypeKind Kind() const override { return this->kind; }

private:
  TypeKind kind;
};

struct FloatType : public Type {
  explicit FloatType(TypeKind kind) : kind(kind) {
    assert(kind == TypeKind::Float32 || kind == TypeKind::Float64);
  }

  TypeKind Kind() const override { return this->kind; }

private:
  TypeKind kind;
};

struct BoolType : public Type {
  explicit BoolType() {}

  TypeKind Kind() const override { return TypeKind::Bool; }
};

struct CharType : public Type {
  explicit CharType() {}

  TypeKind Kind() const override { return TypeKind::Char; }
};

struct StrType : public Type {
  explicit StrType() {}

  TypeKind Kind() const override { return TypeKind::Str; }
};

struct SliceType;
struct ArrayType;
struct RefType;
struct PointerType;

struct TypeNode {
public:
  TypeNode() = delete;

  template <typename T> explicit TypeNode(T value) : value(value) {}

  TypeKind Kind() const { return value->Kind(); }
  auto Value() const { return value; }

  auto AsSInt() const { return (SIntType *)value; }

  auto AsUInt() const { return (UIntType *)value; }

  auto AsFloat() const { return (FloatType *)value; }

  auto AsBool() const { return (BoolType *)value; }

  auto AsChar() const { return (CharType *)value; }

  auto AsStr() const { return (StrType *)value; }

  auto AsSlice() const { return (SliceType *)value; }

  auto AsArray() const { return (ArrayType *)value; }

  auto AsRef() const { return (RefType *)value; }

  auto AsPointer() const { return (PointerType *)value; }

private:
  Type *value;
};

struct SliceType : public Type {
  explicit SliceType(const TypeNode &type)
      : type(std::make_shared<TypeNode>(type)) {}

  TypeKind Kind() const override { return TypeKind::Slice; }
  auto Inner() const { return this->type; }

private:
  std::shared_ptr<TypeNode> type;
};

struct ArrayType : public Type {
  ArrayType(const TypeNode &type, uint64_t size)
      : type(std::make_shared<TypeNode>(type)), size(size) {}

  TypeKind Kind() const override { return TypeKind::Array; }
  auto Inner() const { return this->type; }
  auto Size() const { return this->size; }

private:
  std::shared_ptr<TypeNode> type;
  uint64_t size;
};

struct RefType : public Type {
  explicit RefType(const TypeNode &type)
      : type(std::make_shared<TypeNode>(type)) {}

  TypeKind Kind() const override { return TypeKind::Ref; }
  auto Inner() const { return this->type; }

private:
  std::shared_ptr<TypeNode> type;
};

struct PointerType : public Type {
  explicit PointerType(const TypeNode &type)
      : type(std::make_shared<TypeNode>(type)) {}

  TypeKind Kind() const override { return TypeKind::Pointer; }
  auto Inner() const { return this->type; }

private:
  std::shared_ptr<TypeNode> type;
};

ParseResult<TypeNode *> ParseType(Parser &parser);

} // namespace aatbe::parser

