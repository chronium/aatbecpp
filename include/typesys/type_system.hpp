//
// Created by chronium on 08.09.2022.
//

#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>
#include <utility>
#include <unordered_map>
#include <functional>

namespace aatbe::typesys {

template <class T, class U>
concept Derived = std::is_base_of<U, T>::value;

class BaseType;
class TypeSystem;

struct TypeId {
  size_t value;

  friend std::hash<TypeId>;

  TypeId(TypeId &&tid) = default;
  TypeId(TypeId const &) = default;
  TypeId &operator=(TypeId const &) = delete;

  TypeId() : value(0) {}
  TypeId(TypeSystem *type_system, size_t value)
      : value(value), type_system(type_system) {}

  static TypeId GetNext(TypeSystem *ts);

  auto Value() const { return value; }

  template <Derived<BaseType> T> T *Resolve() const;

  std::size_t operator()(TypeId const &tid) const noexcept {
    return std::hash<size_t>{}(tid.value);
  }

private:
  TypeSystem *type_system;
};

bool operator==(const TypeId &lhs, const TypeId &rhs) {
  return lhs.value == rhs.value;
}

}

namespace std {

template <> struct hash<aatbe::typesys::TypeId> {
  std::size_t operator()(aatbe::typesys::TypeId const &tid) const noexcept {
    return std::hash<size_t>{}(tid.value);
  }
};
}

namespace aatbe::typesys {

enum TypeKind {
  TyStruct,
  TyInt,
  TyFloat,
  TyBool,
  TyChar,
  TyUnit,
  TyArray,
  // TODO: function type
  TyFunction,
  TyPointer,
  TyVoid,
  TyUnknown
};

class BaseType {
public:
  BaseType() = default;
  BaseType(BaseType &&) = delete;
  BaseType(BaseType const &) = delete;
  BaseType &operator=(BaseType const &) = delete;

  virtual ~BaseType() = default;

  virtual TypeKind Kind() const = 0;
};

class Type {
public:
  Type() = delete;
  Type(Type &&) = delete;
  Type(Type const &) = delete;
  Type &operator=(Type const &) = delete;

  template <Derived<BaseType> T>
  explicit Type(T *type) : type(std::move(type)) {}

  TypeKind Kind() const { return this->type->Kind(); }

  template <Derived<BaseType> T> T *As() {
    return dynamic_cast<T *>(this->type.get());
  }

private:
  std::unique_ptr<BaseType> type;
};

class StructType : public BaseType {
public:
  StructType() = delete;
  StructType(StructType &&) = delete;
  StructType(StructType const &) = delete;
  StructType &operator=(StructType const &) = delete;

  explicit StructType(std::string name) : name(std::move(name)) {}

  std::string Name() const { return this->name; }
  size_t Size() const { return this->fields.size(); }
  auto &Fields() const { return this->fields; }

  void AddField(std::string &&name, TypeId type) {
    this->fields.emplace(std::move(name), type);
  }
  auto GetField(std::string const &name) const { return this->fields.at(name); }

  TypeKind Kind() const override { return TypeKind::TyStruct; }

private:
  std::string name;
  std::unordered_map<std::string, TypeId> fields;
};

class IntType : public BaseType {
public:
  enum class IntSize {
    Int8,
    Int16,
    Int32,
    Int64,
  };

  IntType() = delete;
  IntType(IntType &&) = delete;
  IntType(IntType const &) = delete;
  IntType &operator=(IntType const &) = delete;

  IntType(IntSize size, bool sign = true) : size(size), sign(sign) {}

  IntSize Size() const { return this->size; }
  bool Signed() const { return this->sign; }

  TypeKind Kind() const override { return TypeKind::TyInt; }

private:
  IntSize size;
  bool sign;
};

class FloatType : public BaseType {
public:
  enum class FloatSize {
    Float32,
    Float64,
  };

  FloatType() = delete;
  FloatType(FloatType &&) = delete;
  FloatType(FloatType const &) = delete;
  FloatType &operator=(FloatType const &) = delete;

  explicit FloatType(FloatSize size) : size(size) {}

  FloatSize Size() const { return this->size; }

  TypeKind Kind() const override { return TypeKind::TyFloat; }

private:
  FloatSize size;
};

class BoolType : public BaseType {
public:
  BoolType(BoolType &&) = delete;
  BoolType(BoolType const &) = delete;
  BoolType &operator=(BoolType const &) = delete;

  explicit BoolType() {}

  TypeKind Kind() const override { return TypeKind::TyBool; }
};

class CharType : public BaseType {
public:
  CharType(CharType &&) = delete;
  CharType(CharType const &) = delete;
  CharType &operator=(CharType const &) = delete;

  explicit CharType() {}

  TypeKind Kind() const override { return TypeKind::TyChar; }
};

class UnitType : public BaseType {
public:
  UnitType(UnitType &&) = delete;
  UnitType(UnitType const &) = delete;
  UnitType &operator=(UnitType const &) = delete;

  explicit UnitType() {}

  TypeKind Kind() const override { return TypeKind::TyUnit; }
};

class ArrayType : public BaseType {
public:
  ArrayType() = delete;
  ArrayType(ArrayType &&) = delete;
  ArrayType(ArrayType const &) = delete;
  ArrayType &operator=(ArrayType const &) = delete;

  ArrayType(TypeId type, size_t size) : type(type), size(size) {}

  TypeId Type() const { return this->type; }
  size_t Size() const { return this->size; }

  TypeKind Kind() const override { return TypeKind::TyArray; }

private:
  TypeId type;
  size_t size;
};

class PointerType : public BaseType {
public:
  PointerType() = delete;
  PointerType(PointerType &&) = delete;
  PointerType(PointerType const &) = delete;
  PointerType &operator=(PointerType const &) = delete;

  explicit PointerType(TypeId type) : type(type) {}

  TypeId Type() const { return this->type; }

  TypeKind Kind() const override { return TypeKind::TyPointer; }

private:
  TypeId type;
};

class VoidType : public BaseType {
public:
  VoidType(VoidType &&) = delete;
  VoidType(VoidType const &) = delete;
  VoidType &operator=(VoidType const &) = delete;

  explicit VoidType() {}

  TypeKind Kind() const override { return TypeKind::TyVoid; }
};

class UnknownType : public BaseType {
public:
  UnknownType(UnknownType &&) = delete;
  UnknownType(UnknownType const &) = delete;
  UnknownType &operator=(UnknownType const &) = delete;

  explicit UnknownType() {}

  TypeKind Kind() const override { return TypeKind::TyUnknown; }
};

class TypeSystem {
public:
  TypeSystem() = default;
  TypeSystem(TypeSystem &&) = delete;
  TypeSystem(TypeSystem const &) = delete;
  TypeSystem &operator=(TypeSystem const &) = delete;

  TypeId AddType(Type *type) {
    auto id = TypeId::GetNext(this);
    this->types.emplace(id, type);
    return id;
  }

  Type *GetType(TypeId id) { return this->types.at(id); }

  template <Derived<BaseType> T, typename... Args>
  TypeId Create(Args &&...args) {
    return this->AddType(new Type(new T(args...)));
  }

private:
  std::unordered_map<TypeId, Type *> types{};
};

} // namespace aatbe::typesys
