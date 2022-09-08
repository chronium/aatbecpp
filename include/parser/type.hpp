//
// Created by chronium on 22.08.2022.
//

#pragma once

#include <string>
#include <vector>
#include <cassert>

#include <llvm/IR/Type.h>
#include <codegen.hpp>

using namespace aatbe::codegen;

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
  Typename,
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

  virtual llvm::Type *LLVMType() = 0;
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

  llvm::Type *LLVMType() override {
    switch (kind) {
    case TypeKind::Int8:
      return llvm::Type::getInt8Ty(*LLVMContext);
    case TypeKind::Int16:
      return llvm::Type::getInt16Ty(*LLVMContext);
    case TypeKind::Int32:
      return llvm::Type::getInt32Ty(*LLVMContext);
    case TypeKind::Int64:
      return llvm::Type::getInt64Ty(*LLVMContext);
    default:
      return llvm::Type::getInt64Ty(*LLVMContext);
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

  llvm::Type *LLVMType() override {
    switch (kind) {
    case TypeKind::UInt8:
      return llvm::Type::getInt8Ty(*LLVMContext);
    case TypeKind::UInt16:
      return llvm::Type::getInt16Ty(*LLVMContext);
    case TypeKind::UInt32:
      return llvm::Type::getInt32Ty(*LLVMContext);
    case TypeKind::UInt64:
      return llvm::Type::getInt64Ty(*LLVMContext);
    default:
      return llvm::Type::getInt64Ty(*LLVMContext);
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

  llvm::Type *LLVMType() override {
    switch (kind) {
    case TypeKind::Float32:
      return llvm::Type::getFloatTy(*LLVMContext);
    case TypeKind::Float64:
      return llvm::Type::getDoubleTy(*LLVMContext);
    default:
      return llvm::Type::getFloatTy(*LLVMContext);
    }
  }

private:
  TypeKind kind;
};

struct TypenameType : public Type {
  explicit TypenameType(std::string name) : name(std::move(name)) {}

  TypeKind Kind() const override { return TypeKind::Typename; }
  std::string Name() const { return this->name; }

  std::string Format() const override { return name; }

  llvm::Type *LLVMType() override { return GetCompilerContext()->GetStruct(this->name); }

private:
  std::string name;
};

struct BoolType : public Type {
  explicit BoolType() {}

  TypeKind Kind() const override { return TypeKind::Bool; }

  std::string Format() const override { return "bool"; }

  llvm::Type *LLVMType() override {
    return llvm::Type::getInt1Ty(*LLVMContext);
  }
};

struct CharType : public Type {
  explicit CharType() {}

  TypeKind Kind() const override { return TypeKind::Char; }

  std::string Format() const override { return "char"; }

  llvm::Type *LLVMType() override {
    return llvm::Type::getInt8Ty(*LLVMContext);
  }
};

struct StrType : public Type {
  explicit StrType() {}

  TypeKind Kind() const override { return TypeKind::Str; }

  std::string Format() const override { return "str"; }

  llvm::Type *LLVMType() override {
    return llvm::Type::getInt8PtrTy(*LLVMContext);
  }
};

struct UnitType : public Type {
  explicit UnitType() {}

  TypeKind Kind() const override { return TypeKind::Unit; }

  std::string Format() const override { return "()"; }

  llvm::Type *LLVMType() override {
    return llvm::Type::getVoidTy(*LLVMContext);
  }
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

  llvm::Type *LLVMType() { return value->LLVMType(); }

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

  auto AsTypename() const { return (TypenameType *)value; }

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

  llvm::Type *LLVMType() override {
    return llvm::Type::getVoidTy(*LLVMContext);
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

  llvm::Type *LLVMType() override {
    return llvm::Type::getVoidTy(*LLVMContext);
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

  llvm::Type *LLVMType() override {
    return llvm::Type::getVoidTy(*LLVMContext);
  }

private:
  const TypeNode *type;
};

struct PointerType : public Type {
  explicit PointerType(TypeNode *type) : type(type) {}

  TypeKind Kind() const override { return TypeKind::Pointer; }
  auto Inner() const { return this->type; }

  std::string Format() const override { return "ptr " + this->type->Format(); }

  llvm::Type *LLVMType() override {
    return llvm::PointerType::get(this->type->LLVMType(), 0);
  }

private:
  TypeNode *type;
};

} // namespace aatbe::parser

