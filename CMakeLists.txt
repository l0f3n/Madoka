cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
  COMPILE_FLAGS
  -g
)

project (madoka VERSION 1.0)

set(
  SOURCES
  AST/AST.cc
  AST/Print.cc
  CodeGenerator/CodeGenerator.cc
  Error/Error.cc
  Main.cc
  Parser/Parser.cc
  Quads/Quads.cc
  SymbolTable/Symbol.cc
  SymbolTable/SymbolTable.cc
  Tokenizer/Token.cc
  Tokenizer/Tokenizer.cc
  TypeChecker/TypeChecker.cc
)

set(
  HEADERS
  AST/AST.h
  CodeGenerator/CodeGenerator.h
  Error/Error.h
  Parser/Parser.h
  Quads/Quads.h
  SymbolTable/Symbol.h
  SymbolTable/SymbolTable.h
  Tokenizer/Token.h
  Tokenizer/Tokenizer.h
  TypeChecker/TypeChecker.h
)

add_executable(madoka ${SOURCES} ${HEADERS})

target_include_directories(
  madoka
  PRIVATE .
)
