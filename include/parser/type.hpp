//
// Created by chronium on 22.08.2022.
//

#pragma once

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
  Unit,
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
  virtual std::string Format() const = 0;
};

struct SIntType : public Type {
  explicit SIntType(TypeKind kind) : kind(kind) {
    assert(kind == TypeKind::Int8 || kind == TypeKind::Int16 ||
           kind == TypeKind::Int32 || kind == TypeKind::Int64);
  }

  TypeKind Kind() const override { return this->kind; }

  std::string Format() const override {
    switch (kind) {
    case TypeKind::Int8:
      return "int8";
    case TypeKind::Int16:
      return "int16";
    case TypeKind::Int32:
      return "int32";
    case TypeKind::Int64:
      return "int64";
    default:
      return "int8";
    }
  }

private:
  TypeKind kind;
};

struct UIntType : public Type {
  explicit UIntType(TypeKind kind) : kind(kind) {
    assert(kind == TypeKind::UInt8 || kind == TypeKind::UInt16 ||
           kind == TypeKind::UInt32 || kind == TypeKind::UInt64);
  }

  TypeKind Kind() const override { return this->kind; }

  std::string Format() const override {
    switch (kind) {
    case TypeKind::UInt8:
      return "uint8";
    case TypeKind::UInt16:
      return "uint16";
    case TypeKind::UInt32:
      return "uint32";
    case TypeKind::UInt64:
      return "uint64";
    default:
      return "uint8";
    }
  }

private:
  TypeKind kind;
};

struct FloatType : public Type {
  explicit FloatType(TypeKind kind) : kind(kind) {
    assert(kind == TypeKind::Float32 || kind == TypeKind::Float64);
  }

  TypeKind Kind() const override { return this->kind; }

  std::string Format() const override {
    switch (kind) {
    case TypeKind::Float32:
      return "float32";
    case TypeKind::Float64:
      return "float64";
    default:
      return "float32";
    }
  }

private:
  TypeKind kind;
};

struct BoolType : public Type {
  explicit BoolType() {}

  TypeKind Kind() const override { return TypeKind::Bool; }

  std::string Format() const override { return "bool"; }
};

struct CharType : public Type {
  explicit CharType() {}

  TypeKind Kind() const override { return TypeKind::Char; }

  std::string Format() const override { return "char"; }
};

struct StrType : public Type {
  explicit StrType() {}

  TypeKind Kind() const override { return TypeKind::Str; }

  std::string Format() const override { return "str"; }
};

struct UnitType : public Type {
  explicit UnitType() {}

  TypeKind Kind() const override { return TypeKind::Unit; }

  std::string Format() const override { return "()"; }
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
  auto Format() const { return "Type(" + value->Format() + ")"; }

  auto AsSInt() const { return (SIntType *)value; }

  auto AsUInt() const { return (UIntType *)value; }

  auto AsFloat() const { return (FloatType *)value; }

  auto AsBool() const { return (BoolType *)value; }

  auto AsChar() const { return (CharType *)value; }

  auto AsStr() const { return (StrType *)value; }

  auto AsUnit() const { return (UnitType *)value; }

  auto AsSlice() const { return (SliceType *)value; }

  auto AsArray() const { return (ArrayType *)value; }

  auto AsRef() const { return (RefType *)value; }

  auto AsPointer() const { return (PointerType *)value; }

private:
  Type *value;
};

struct SliceType : public Type {
  explicit SliceType(const TypeNode *type) : type(type) {}

  TypeKind Kind() const override { return TypeKind::Slice; }
  auto Inner() const { return this->type; }

  std::string Format() const override {
    return "[" + this->type->Format() + "]";
  }

private:
  const TypeNode *type;
};

struct ArrayType : public Type {
  ArrayType(const TypeNode *type, uint64_t size) : type(type), size(size) {}

  TypeKind Kind() const override { return TypeKind::Array; }
  auto Inner() const { return this->type; }
  auto Size() const { return this->size; }

  std::string Format() const override {
    return "[" + this->type->Format() + "; " + std::to_string(this->size) + "]";
  }

private:
  const TypeNode *type;
  uint64_t size;
};

struct RefType : public Type {
  explicit RefType(const TypeNode *type) : type(type) {}

  TypeKind Kind() const override { return TypeKind::Ref; }
  auto Inner() const { return this->type; }

  std::string Format() const override { return "ref " + this->type->Format(); }

private:
  const TypeNode *type;
};

struct PointerType : public Type {
  explicit PointerType(const TypeNode *type) : type(type) {}

  TypeKind Kind() const override { return TypeKind::Pointer; }
  auto Inner() const { return this->type; }

  std::string Format() const override { return "ptr " + this->type->Format(); }

private:
  const TypeNode *type;
};

} // namespace aatbe::parser

