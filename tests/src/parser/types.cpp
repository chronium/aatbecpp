//
// Created by chronium on 22.08.2022.
//

#include <gtest/gtest.h>

#include <parser/type.hpp>

#include "base.hpp"

using namespace aatbe::parser;

TEST(TypeParser, Int) {
  auto tokens = makeTokens("int8 int16 int32 int64");
  Parser parser(tokens);

  auto int8 = ParseType(parser);
  auto int16 = ParseType(parser);
  auto int32 = ParseType(parser);
  auto int64 = ParseType(parser);

  EXPECT_TRUE(int8);
  EXPECT_EQ(int8.Kind(), TypeKind::Int8);

  EXPECT_TRUE(int16);
  EXPECT_EQ(int16.Kind(), TypeKind::Int16);

  EXPECT_TRUE(int32);
  EXPECT_EQ(int32.Kind(), TypeKind::Int32);

  EXPECT_TRUE(int64);
  EXPECT_EQ(int64.Kind(), TypeKind::Int64);
}

TEST(TypeParser, UInt) {
  auto tokens = makeTokens("uint8 uint16 uint32 uint64");
  Parser parser(tokens);

  auto uint8 = ParseType(parser);
  auto uint16 = ParseType(parser);
  auto uint32 = ParseType(parser);
  auto uint64 = ParseType(parser);

  EXPECT_TRUE(uint8);
  EXPECT_EQ(uint8.Kind(), TypeKind::UInt8);

  EXPECT_TRUE(uint16);
  EXPECT_EQ(uint16.Kind(), TypeKind::UInt16);

  EXPECT_TRUE(uint32);
  EXPECT_EQ(uint32.Kind(), TypeKind::UInt32);

  EXPECT_TRUE(uint64);
  EXPECT_EQ(uint64.Kind(), TypeKind::UInt64);
}

TEST(TypeParser, Float) {
  auto tokens = makeTokens("float32 float64");
  Parser parser(tokens);

  auto float32 = ParseType(parser);
  auto float64 = ParseType(parser);

  EXPECT_TRUE(float32);
  EXPECT_EQ(float32.Kind(), TypeKind::Float32);

  EXPECT_TRUE(float64);
  EXPECT_EQ(float64.Kind(), TypeKind::Float64);
}

TEST(TypeParser, Bool) {
  auto tokens = makeTokens("bool");
  Parser parser(tokens);

  auto bool_ = ParseType(parser);

  EXPECT_TRUE(bool_);
  EXPECT_EQ(bool_.Kind(), TypeKind::Bool);
}

TEST(TypeParser, Char) {
  auto tokens = makeTokens("char");
  Parser parser(tokens);

  auto char_ = ParseType(parser);

  EXPECT_TRUE(char_);
  EXPECT_EQ(char_.Kind(), TypeKind::Char);
}

TEST(TypeParser, String) {
  auto tokens = makeTokens("str");
  Parser parser(tokens);

  auto str = ParseType(parser);

  EXPECT_TRUE(str);
  EXPECT_EQ(str.Kind(), TypeKind::Str);
}

TEST(TypeParser, Slice) {
  auto tokens = makeTokens("[int8]");
  Parser parser(tokens);

  auto slice = ParseType(parser);

  EXPECT_TRUE(slice);
  EXPECT_EQ(slice.Kind(), TypeKind::Slice);
  EXPECT_EQ(slice.Node().AsSlice()->Inner()->Kind(), TypeKind::Int8);
}

TEST(TypeParser, Array) {
  auto tokens = makeTokens("[int8; 128]");
  Parser parser(tokens);

  auto array = ParseType(parser);

  EXPECT_TRUE(array);
  EXPECT_EQ(array.Kind(), TypeKind::Array);
  EXPECT_EQ(array.Node().AsArray()->Inner()->Kind(), TypeKind::Int8);
  EXPECT_EQ(array.Node().AsArray()->Size(), 128);
}

TEST(TypeParser, Ref) {
  auto tokens = makeTokens("&int8 &&str");
  Parser parser(tokens);

  auto ref = ParseType(parser);
  auto refRef = ParseType(parser);

  EXPECT_TRUE(ref);
  EXPECT_EQ(ref.Kind(), TypeKind::Ref);
  EXPECT_EQ(ref.Node().AsRef()->Inner()->Kind(), TypeKind::Int8);

  EXPECT_TRUE(refRef);
  EXPECT_EQ(refRef.Kind(), TypeKind::Ref);
  EXPECT_EQ(refRef.Node().AsRef()->Inner()->Kind(), TypeKind::Ref);
}

TEST(TypeParser, Pointer) {
  auto tokens = makeTokens("*int8 **str");
  Parser parser(tokens);

  auto pointer = ParseType(parser);
  auto pointerPointer = ParseType(parser);

  EXPECT_TRUE(pointer);
  EXPECT_EQ(pointer.Kind(), TypeKind::Pointer);
  EXPECT_EQ(pointer.Node().AsPointer()->Inner()->Kind(), TypeKind::Int8);

  EXPECT_TRUE(pointerPointer);
  EXPECT_EQ(pointerPointer.Kind(), TypeKind::Pointer);
  EXPECT_EQ(pointerPointer.Node().AsPointer()->Inner()->Kind(),
            TypeKind::Pointer);
}
