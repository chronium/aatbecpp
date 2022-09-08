//
// Created by chronium on 08.09.2022.
//

#include <gtest/gtest.h>

#include <typesys/type_system.hpp>

using namespace aatbe::typesys;

TEST(TypeSystem, EmptyStruct) {
  TypeSystem ts{};

  auto tid = ts.Create<StructType>("Test");
  auto ty_struct = ts.GetType(tid)->As<StructType>();

  EXPECT_EQ(ty_struct->Kind(), TypeKind::TyStruct);
  EXPECT_EQ(ty_struct->Name(), "Test");
  EXPECT_EQ(ty_struct->Size(), 0);
}

TEST(TypeSystem, Newtype) {
  TypeSystem ts{};

  auto tid = ts.Create<StructType>("Test");
  auto ty_struct = ts.GetType(tid)->As<StructType>();

  ty_struct->AddField("a", ts.Create<IntType>(IntType::IntSize::Int32));

  EXPECT_EQ(ty_struct->Kind(), TypeKind::TyStruct);
  EXPECT_EQ(ty_struct->Name(), "Test");
  EXPECT_EQ(ty_struct->Size(), 1);
  EXPECT_EQ(ty_struct->GetField("a").Resolve<IntType>()->Size(),
            IntType::IntSize::Int32);
}

TEST(TypeSystem, NestedStructs) {
  TypeSystem ts{};

  auto tid = ts.Create<StructType>("Test");
  auto ty_struct = ts.GetType(tid)->As<StructType>();

  ty_struct->AddField("a", ts.Create<IntType>(IntType::IntSize::Int32));
  ty_struct->AddField("b", ts.Create<StructType>("Nested"));

  EXPECT_EQ(ty_struct->Kind(), TypeKind::TyStruct);
  EXPECT_EQ(ty_struct->Name(), "Test");
  EXPECT_EQ(ty_struct->Size(), 2);
  EXPECT_EQ(ty_struct->GetField("a").Resolve<IntType>()->Size(),
            IntType::IntSize::Int32);
  EXPECT_EQ(ty_struct->GetField("b").Resolve<StructType>()->Name(), "Nested");
}
